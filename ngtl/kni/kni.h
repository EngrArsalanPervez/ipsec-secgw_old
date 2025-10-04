#ifndef _KNI_H_
#define _KNI_H_

#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_kni.h>
#include <rte_mempool.h>
#include <stdint.h>

/* Max kernels threads per port */
#define KNI_MAX_KTHREAD 32

#define KNI_CORE0 4
#define KNI_CORE1 5

/*
 * Structure of port parameters
 */
struct kni_port_params {
  uint16_t port_id;    /* Port ID */
  unsigned lcore_rx;   /* lcore ID for RX */
  unsigned lcore_tx;   /* lcore ID for TX */
  uint32_t nb_lcore_k; /* Number of lcores for KNI multi kernel threads */
  uint32_t nb_kni;     /* Number of KNI devices to be created */
  unsigned lcore_k[KNI_MAX_KTHREAD];    /* lcore ID list for kthreads */
  struct rte_kni *kni[KNI_MAX_KTHREAD]; /* KNI context pointers */
} __rte_cache_aligned;

/* Structure type for recording kni interface specific stats */
struct kni_interface_stats {
  /* number of pkts received from NIC, and sent to KNI */
  uint64_t rx_packets;

  /* number of pkts received from NIC, but failed to send to KNI */
  uint64_t rx_dropped;

  /* number of pkts received from KNI, and sent to NIC */
  uint64_t tx_packets;

  /* number of pkts received from KNI, but failed to send to NIC */
  uint64_t tx_dropped;
};

/* Globals (defined in kni.c) */
extern struct kni_port_params *kni_port_params_array[RTE_MAX_ETHPORTS];
extern struct rte_mempool *pktmbuf_pool;
extern uint32_t ports_mask;
extern int promiscuous_on_kni;
extern volatile uint32_t kni_stop;
extern volatile uint32_t kni_pause;
extern int monitor_links;
int kni_main(struct rte_mempool *shared_pool);
int kni_free_kni(uint16_t port_id);
void signal_handler_kni(void);
void kni_burst_free_mbufs(struct rte_mbuf **pkts, unsigned num);
void kni_ingress(struct kni_port_params *p);
void kni_egress(struct kni_port_params *p);
int main_loop(void *arg);
void handle_kni_packet(struct rte_mbuf *buff);
// static uint32_t parse_unsigned(const char *portmask);
void print_config(void);
int parse_config_kni(const char *arg);
// int validate_parameters(uint32_t portmask);
void init_kni(void);
void init_port(uint16_t port);

static int kni_change_mtu(uint16_t port_id, unsigned int new_mtu);
int kni_config_network_interface(uint16_t port_id, uint8_t if_up);
int kni_config_mac_address(uint16_t port_id, uint8_t mac_addr[]);

void print_ethaddr1(const char *name, struct rte_ether_addr *mac_addr);
int kni_alloc(uint16_t port_id);
int kni_free_kni(uint16_t port_id);
int kni_main_loop(__rte_unused void *arg);
#endif /* _KNI_H_ */
