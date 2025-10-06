#include "kni.h"
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <rte_branch_prediction.h>
#include <rte_bus_pci.h>
#include <rte_common.h>
#include <rte_cycles.h>
#include <rte_debug.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_interrupts.h>
#include <rte_kni.h>
#include <rte_launch.h>
#include <rte_lcore.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_mbuf.h>
#include <rte_memcpy.h>
#include <rte_memory.h>
#include <rte_mempool.h>
#include <rte_per_lcore.h>
#include <rte_string_fns.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/queue.h>
#include <unistd.h>

#include "../../ipsec.h"
#include "../../ipsec_worker.h"

struct ipEncryptorTypeStruct ipEncryptorType = {0};

#define MAX_PACKET_SZ 2048
#define PKT_BURST_SZ 32
#define NB_RXD 1024
#define NB_TXD 1024

uint8_t kni_configured = 0;
#define RTE_LOGTYPE_APP RTE_LOGTYPE_USER1
struct kni_port_params *kni_port_params_array[RTE_MAX_ETHPORTS];
uint32_t ports_mask = 0;
int promiscuous_on_kni = 0;
volatile uint32_t kni_stop = 0;
volatile uint32_t kni_pause = 0;
struct kni_interface_stats kni_stats[RTE_MAX_ETHPORTS];
struct rte_mempool *pktmbuf_pool = NULL;
static struct rte_eth_conf port_conf = {
    .txmode =
        {
            .mq_mode = RTE_ETH_MQ_TX_NONE,
        },
};

void print_kni_stats(void) {
  printf("\nrx_packets:: %35" PRIu64 "\ntx_packets:: %35" PRIu64
         "\nrx_dropped:: %35" PRIu64 "\ntx_dropped:: %35" PRIu64,
         kni_stats[0].rx_packets, kni_stats[0].tx_packets,
         kni_stats[0].rx_dropped, kni_stats[0].tx_dropped);
}

void signal_handler_kni(void) {
  __atomic_fetch_add(&kni_stop, 1, __ATOMIC_RELAXED);
  return;
}

void kni_burst_free_mbufs(struct rte_mbuf **pkts, unsigned num) {
  unsigned i;

  if (pkts == NULL)
    return;

  for (i = 0; i < num; i++) {
    rte_pktmbuf_free(pkts[i]);
    pkts[i] = NULL;
  }
}

// rx from eth, tx to kni
void kni_ingress(struct kni_port_params *p) {
  if (p == NULL)
    return;

  uint8_t i;
  uint32_t nb_kni = p->nb_kni;

  for (i = 0; i < nb_kni; i++) {
    // Process kernel-side requests (e.g., change MTU, MAC, link up/down)
    rte_kni_handle_request(p->kni[i]);
  }
}

void kni_filter_ike_packets(int32_t nb_rx, struct rte_mbuf **pkts,
                            uint16_t port_id, struct route_table *rt) {
  int32_t i;
  struct rte_mbuf *m;

  for (i = 0; i < nb_rx; i++) {
    m = pkts[i];
    struct rte_ether_hdr *eth = rte_pktmbuf_mtod(m, struct rte_ether_hdr *);

    if (rte_be_to_cpu_16(eth->ether_type) != RTE_ETHER_TYPE_IPV4) {
      rte_pktmbuf_free(m);
      continue;
    }

    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(eth + 1);

    if ((ip->version_ihl >> 4) != 4 || ip->next_proto_id != IPPROTO_UDP) {
      rte_pktmbuf_free(m);
      continue;
    }

    uint16_t ip_hdr_len = (ip->version_ihl & 0x0f) * 4;
    struct rte_udp_hdr *udp = (struct rte_udp_hdr *)((char *)ip + ip_hdr_len);

    uint16_t sport = rte_be_to_cpu_16(udp->src_port);
    uint16_t dport = rte_be_to_cpu_16(udp->dst_port);

    if (sport == 500 || dport == 500 || sport == 4500 || dport == 4500) {
      // Send Out

      uint16_t tunnel_port = get_route(m, rt, PKT_TYPE_PLAIN_IPV4);

      /* Burst tx to eth */
      uint8_t nb_tx = rte_eth_tx_burst(tunnel_port, 1, &m, 1);
      if (nb_tx)
        kni_stats[port_id].tx_packets += nb_tx;
      if (unlikely(nb_tx < 1)) {
        /* Free mbufs not tx to NIC */
        kni_burst_free_mbufs(&m, 1);
        kni_stats[port_id].tx_dropped += 1;
      }

    } else {
      rte_pktmbuf_free(m);
      continue;
    }
  }
}

// rx from kni, tx to eth
void kni_egress(struct kni_port_params *p, struct route_table *rt) {
  uint8_t i;
  uint16_t port_id;
  unsigned nb_tx, num;
  uint32_t nb_kni;
  struct rte_mbuf *pkts_burst[PKT_BURST_SZ];

  if (p == NULL)
    return;

  nb_kni = p->nb_kni;
  port_id = p->port_id;
  for (i = 0; i < nb_kni; i++) {
    /* Burst rx from kni */
    num = rte_kni_rx_burst(p->kni[i], pkts_burst, PKT_BURST_SZ);
    if (unlikely(num > PKT_BURST_SZ)) {
      RTE_LOG(ERR, APP, "Error receiving from KNI\n");
      return;
    }

    // Only send IKE Traffic
    kni_filter_ike_packets(num, pkts_burst, port_id, rt);
  }
}

int main_loop(void *arg) {
  uint16_t i;
  int32_t f_stop;
  int32_t f_pause;
  const unsigned lcore_id = rte_lcore_id();
  enum lcore_rxtx { LCORE_NONE, LCORE_RX, LCORE_TX, LCORE_MAX };
  enum lcore_rxtx flag = LCORE_NONE;

  RTE_ETH_FOREACH_DEV(i) {
    if (!kni_port_params_array[i])
      continue;
    if (kni_port_params_array[i]->lcore_rx == (uint8_t)lcore_id) {
      flag = LCORE_RX;
      break;
    } else if (kni_port_params_array[i]->lcore_tx == (uint8_t)lcore_id) {
      flag = LCORE_TX;
      break;
    }
  }

  struct route_table rt;
  rt.rt4_ctx = socket_ctx[0 /*socket_id*/].rt_ip4;
  rt.rt6_ctx = socket_ctx[0 /*socket_id*/].rt_ip6;

  if (flag == LCORE_RX) {
    RTE_LOG(INFO, APP, "Lcore %u is reading from port %d\n",
            kni_port_params_array[i]->lcore_rx,
            kni_port_params_array[i]->port_id);
    while (1) {
      f_stop = __atomic_load_n(&kni_stop, __ATOMIC_RELAXED);
      f_pause = __atomic_load_n(&kni_pause, __ATOMIC_RELAXED);
      if (f_stop)
        break;
      if (f_pause)
        continue;
      kni_ingress(kni_port_params_array[i]);
      rte_delay_us_sleep(10000); // sleep for 100 µs
    }
  } else if (flag == LCORE_TX) {
    RTE_LOG(INFO, APP, "Lcore %u is writing to port %d\n",
            kni_port_params_array[i]->lcore_tx,
            kni_port_params_array[i]->port_id);
    while (1) {
      f_stop = __atomic_load_n(&kni_stop, __ATOMIC_RELAXED);
      f_pause = __atomic_load_n(&kni_pause, __ATOMIC_RELAXED);
      if (f_stop)
        break;
      if (f_pause)
        continue;
      kni_egress(kni_port_params_array[i], &rt);
      rte_delay_us_sleep(100); // sleep for 100 µs
    }
  } else
    RTE_LOG(INFO, APP, "Lcore %u has nothing to do\n", lcore_id);

  return 0;
}

int parse_config_kni(const char *arg) {
  const char *p, *p0 = arg;
  char s[256];
  unsigned size;
  int i, j, nb_token;
  char *str_fld[KNI_MAX_KTHREAD + 3];
  unsigned long int_fld[KNI_MAX_KTHREAD + 3];
  uint16_t port_id;

  memset(&kni_port_params_array, 0, sizeof(kni_port_params_array));

  while ((p = strchr(p0, '(')) != NULL) {
    p++;
    if ((p0 = strchr(p, ')')) == NULL)
      return -1;

    size = p0 - p;
    if (size >= sizeof(s))
      return -1;

    snprintf(s, sizeof(s), "%.*s", size, p);

    nb_token = rte_strsplit(s, sizeof(s), str_fld, KNI_MAX_KTHREAD + 3, ',');
    if (nb_token < 3)
      return -1;

    for (i = 0; i < nb_token; i++)
      int_fld[i] = strtoul(str_fld[i], NULL, 0);

    i = 0;
    port_id = int_fld[i++];
    if (port_id >= RTE_MAX_ETHPORTS)
      return -1;

    if (kni_port_params_array[port_id])
      return -1;

    kni_port_params_array[port_id] = rte_zmalloc(
        "KNI_port_params", sizeof(struct kni_port_params), RTE_CACHE_LINE_SIZE);

    kni_port_params_array[port_id]->port_id = port_id;
    kni_port_params_array[port_id]->lcore_rx = (uint8_t)int_fld[i++];
    kni_port_params_array[port_id]->lcore_tx = (uint8_t)int_fld[i++];

    for (j = 0; i < nb_token && j < KNI_MAX_KTHREAD; i++, j++)
      kni_port_params_array[port_id]->lcore_k[j] = (uint8_t)int_fld[i];

    kni_port_params_array[port_id]->nb_lcore_k = j;
  }

  return 0;
}

#define CMDLINE_OPT_CONFIG "config"

void init_kni(void) {
  unsigned int num_of_kni_ports = 0, i;
  struct kni_port_params **params = kni_port_params_array;
  for (i = 0; i < RTE_MAX_ETHPORTS; i++) {
    if (kni_port_params_array[i]) {
      num_of_kni_ports += (params[i]->nb_lcore_k ? params[i]->nb_lcore_k : 1);
    }
  }
  rte_kni_init(num_of_kni_ports);
}

int kni_change_mtu(uint16_t port_id, unsigned int new_mtu) {
  int ret;
  uint16_t nb_rxd = NB_RXD;
  uint16_t nb_txd = NB_TXD;
  struct rte_eth_conf conf;
  struct rte_eth_dev_info dev_info;
  struct rte_eth_rxconf rxq_conf;
  struct rte_eth_txconf txq_conf;

  if (!rte_eth_dev_is_valid_port(port_id)) {
    RTE_LOG(ERR, APP, "Invalid port id %d\n", port_id);
    return -EINVAL;
  }

  RTE_LOG(INFO, APP, "Change MTU of port %d to %u\n", port_id, new_mtu);
  ret = rte_eth_dev_stop(port_id);
  if (ret != 0) {
    RTE_LOG(ERR, APP, "Failed to stop port %d: %s\n", port_id,
            rte_strerror(-ret));
    return ret;
  }
  memcpy(&conf, &port_conf, sizeof(conf));
  conf.rxmode.mtu = new_mtu;
  ret = rte_eth_dev_configure(port_id, 1, 1, &conf);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to reconfigure port %d\n", port_id);
    return ret;
  }

  ret = rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, &nb_txd);
  if (ret < 0)
    rte_exit(EXIT_FAILURE,
             "Could not adjust number of descriptors "
             "for port%u (%d)\n",
             (unsigned int)port_id, ret);

  ret = rte_eth_dev_info_get(port_id, &dev_info);
  if (ret != 0) {
    RTE_LOG(ERR, APP, "Error during getting device (port %u) info: %s\n",
            port_id, strerror(-ret));

    return ret;
  }

  rxq_conf = dev_info.default_rxconf;
  rxq_conf.offloads = conf.rxmode.offloads;
  ret =
      rte_eth_rx_queue_setup(port_id, 0, nb_rxd, rte_eth_dev_socket_id(port_id),
                             &rxq_conf, pktmbuf_pool);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to setup Rx queue of port %d\n", port_id);
    return ret;
  }

  txq_conf = dev_info.default_txconf;
  txq_conf.offloads = conf.txmode.offloads;
  ret = rte_eth_tx_queue_setup(port_id, 0, nb_txd,
                               rte_eth_dev_socket_id(port_id), &txq_conf);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to setup Tx queue of port %d\n", port_id);
    return ret;
  }
  ret = rte_eth_dev_start(port_id);
  if (ret < 0) {
    RTE_LOG(ERR, APP, "Fail to restart port %d\n", port_id);
    return ret;
  }

  return 0;
}

int kni_config_network_interface(uint16_t port_id, uint8_t if_up) {
  int ret = 0;

  if (!rte_eth_dev_is_valid_port(port_id)) {
    RTE_LOG(ERR, APP, "Invalid port id %d\n", port_id);
    return -EINVAL;
  }

  RTE_LOG(INFO, APP, "Configure network interface of %d %s\n", port_id,
          if_up ? "up" : "down");

  __atomic_fetch_add(&kni_pause, 1, __ATOMIC_RELAXED);

  if (if_up != 0) {
    ret = rte_eth_dev_stop(port_id);
    if (ret != 0) {
      RTE_LOG(ERR, APP, "Failed to stop port %d: %s\n", port_id,
              rte_strerror(-ret));
      __atomic_fetch_sub(&kni_pause, 1, __ATOMIC_RELAXED);
      return ret;
    }
    ret = rte_eth_dev_start(port_id);
  } else {
    ret = rte_eth_dev_stop(port_id);
    if (ret != 0) {
      RTE_LOG(ERR, APP, "Failed to stop port %d: %s\n", port_id,
              rte_strerror(-ret));
      __atomic_fetch_sub(&kni_pause, 1, __ATOMIC_RELAXED);
      return ret;
    }
  }

  __atomic_fetch_sub(&kni_pause, 1, __ATOMIC_RELAXED);

  if (ret < 0)
    RTE_LOG(ERR, APP, "Failed to start port %d\n", port_id);

  return ret;
}

void print_ethaddr1(const char *name, struct rte_ether_addr *mac_addr) {
  char buf[RTE_ETHER_ADDR_FMT_SIZE];
  rte_ether_format_addr(buf, RTE_ETHER_ADDR_FMT_SIZE, mac_addr);
  RTE_LOG(INFO, APP, "\t%s%s\n", name, buf);
}

int kni_config_mac_address(uint16_t port_id, uint8_t mac_addr[]) {
  int ret = 0;

  if (!rte_eth_dev_is_valid_port(port_id)) {
    RTE_LOG(ERR, APP, "Invalid port id %d\n", port_id);
    return -EINVAL;
  }

  RTE_LOG(INFO, APP, "Configure mac address of %d\n", port_id);
  print_ethaddr1("Address:", (struct rte_ether_addr *)mac_addr);

  ret = rte_eth_dev_default_mac_addr_set(port_id,
                                         (struct rte_ether_addr *)mac_addr);
  if (ret < 0)
    RTE_LOG(ERR, APP, "Failed to config mac_addr for port %d\n", port_id);

  return ret;
}

int kni_alloc(uint16_t port_id) {
  uint8_t i;
  struct rte_kni *kni;
  struct rte_kni_conf conf;
  struct kni_port_params **params = kni_port_params_array;
  int ret;

  if (port_id >= RTE_MAX_ETHPORTS || !params[port_id])
    return -1;

  params[port_id]->nb_kni =
      params[port_id]->nb_lcore_k ? params[port_id]->nb_lcore_k : 1;

  for (i = 0; i < params[port_id]->nb_kni; i++) {
    memset(&conf, 0, sizeof(conf));

    if (params[port_id]->nb_lcore_k) {
      snprintf(conf.name, RTE_KNI_NAMESIZE, "vEth%u_%u", port_id, i);
      conf.core_id = params[port_id]->lcore_k[i];
      conf.force_bind = 1;
    } else {
      snprintf(conf.name, RTE_KNI_NAMESIZE, "vEth%u", port_id);
    }

    conf.group_id = port_id;
    conf.mbuf_size = MAX_PACKET_SZ;

    if (i == 0) {
      struct rte_kni_ops ops;
      struct rte_eth_dev_info dev_info;

      ret = rte_eth_dev_info_get(port_id, &dev_info);
      if (ret != 0)
        rte_exit(EXIT_FAILURE, "Error getting device info (port %u): %s\n",
                 port_id, strerror(-ret));

      ret =
          rte_eth_macaddr_get(port_id, (struct rte_ether_addr *)&conf.mac_addr);
      if (ret != 0)
        rte_exit(EXIT_FAILURE, "Failed to get MAC address (port %u): %s\n",
                 port_id, rte_strerror(-ret));

      struct rte_ether_addr hardcoded_mac;
      memcpy(hardcoded_mac.addr_bytes, ipEncryptorType.vEth0_0_MAC,
             RTE_ETHER_ADDR_LEN);

      rte_ether_addr_copy(&hardcoded_mac,
                          (struct rte_ether_addr *)&conf.mac_addr);

      rte_eth_dev_get_mtu(port_id, &conf.mtu);

      conf.min_mtu = dev_info.min_mtu;
      conf.max_mtu = dev_info.max_mtu;

      memset(&ops, 0, sizeof(ops));
      ops.port_id = port_id;
      ops.change_mtu = kni_change_mtu;
      ops.config_network_if = kni_config_network_interface;
      ops.config_mac_address = kni_config_mac_address;

      kni = rte_kni_alloc(pktmbuf_pool, &conf, &ops);
    } else {
      kni = rte_kni_alloc(pktmbuf_pool, &conf, NULL);
    }

    if (!kni) {
      rte_exit(EXIT_FAILURE, "Fail to create KNI for port %u\n", port_id);
    }

    RTE_LOG(INFO, APP, "KNI created: %s for port %u (pool=%p)\n", conf.name,
            port_id, (void *)pktmbuf_pool);

    params[port_id]->kni[i] = kni;
  }

  return 0;
}

static uint32_t parse_unsigned(const char *portmask) {
  char *end = NULL;
  unsigned long num;

  num = strtoul(portmask, &end, 16);
  if ((portmask[0] == '\0') || (end == NULL) || (*end != '\0'))
    return 0;

  return (uint32_t)num;
}

int kni_main(struct rte_mempool *shared_pool) {
  const unsigned lcore_id = rte_lcore_id();
  if (lcore_id == ipEncryptorType.kni_rx_core) {
    int ret;
    uint16_t nb_sys_ports, port;
    if (shared_pool == NULL) {
      rte_exit(EXIT_FAILURE, "KNI: no mbuf pool provided\n");
    }
    pktmbuf_pool = shared_pool;
    RTE_LOG(INFO, APP, "KNI: using shared mbuf pool provided by caller\n");
    init_kni();
    ports_mask = parse_unsigned("0x1");
    promiscuous_on_kni = 1;

    char config_kni[32] = {0};
    sprintf(config_kni, "(0,%d,%d,1)", ipEncryptorType.kni_rx_core,
            ipEncryptorType.kni_tx_core);
    ret = parse_config_kni(config_kni);
    if (ret < 0)
      rte_exit(EXIT_FAILURE, "Invalid KNI config\n");

    kni_alloc(ipEncryptorType.client_port);

    kni_configured = 1;

    main_loop(NULL);
  } else {
    while (kni_configured == 0) {
      sleep(1);
    }
    main_loop(NULL);
  }

  return 0;
}
