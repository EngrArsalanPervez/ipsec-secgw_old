#ifndef _KNI_H_
#define _KNI_H_

#include <rte_ethdev.h>
#include <rte_ether.h>
#include <rte_kni.h>
#include <rte_mempool.h>
#include <stdint.h>

#define HCLOSE 0
#define LCLOSE 1

#define IP_ENCRYPTOR_TYPE HCLOSE

#if IP_ENCRYPTOR_TYPE == HCLOSE
#define CLIENT_PORT 0
#define TUNNEL_PORT 1
#define vEth0_0_MAC {0x02, 0x00, 0x00, 0x00, 0x00, 0x01}
#elif IP_ENCRYPTOR_TYPE == LCLOSE
#define CLIENT_PORT 1
#define TUNNEL_PORT 0
#define vEth0_0_MAC {0x02, 0x00, 0x00, 0x00, 0x00, 0x02}
#endif

/* Max kernels threads per port */
#define KNI_MAX_KTHREAD 32

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

int kni_main(struct rte_mempool *shared_pool);
void signal_handler_kni(void);
void print_kni_stats(void);

#endif /* _KNI_H_ */
