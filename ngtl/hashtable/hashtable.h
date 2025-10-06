#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <rte_hash.h>
#include <rte_hash_crc.h>

#define NETSTAT_ENTRIES 128 // 16777216

extern struct rte_hash *NETSTAT;

struct hashStats {
  uint64_t counts;
  uint64_t hits;
};

struct netstatHashKeyStruct {
  uint32_t srcIP;
  uint32_t dstIP;
  uint8_t proto;
  uint16_t srcPort;
  uint16_t dstPort;
};

struct netstatStruct {
  struct netstatHashKeyStruct key;
  uint8_t status;
  uint32_t srcIP;
  uint32_t dstIP;
  uint8_t proto;
  uint16_t srcPort;
  uint16_t dstPort;
  uint8_t inPort;
  uint8_t outPort;
  uint64_t lastPktTime;
};

void MyHashesSetup(void);

#endif
