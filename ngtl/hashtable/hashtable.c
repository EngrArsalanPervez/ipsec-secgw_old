#include "hashtable.h"

struct rte_hash_parameters NETSTAT_params0 = {
    .name = "NETSTAT",
    .entries = NETSTAT_ENTRIES,
    .key_len = sizeof(struct netstatHashKeyStruct),
    .hash_func = rte_hash_crc,
    .hash_func_init_val = 0,
    .socket_id = 0,
};
struct rte_hash *NETSTAT;

void MyHashesSetup(void) {
  NETSTAT = rte_hash_create(&NETSTAT_params0);
  if (NETSTAT == NULL) {
    printf("NETSTAT Memory Allocation Failed in hashsetup.c\n");
    exit(1);
  }
}
