#ifndef _DB_H_
#define _DB_H_

#include <rte_byteorder.h>

#include <bson/bson.h>
#include <curl/curl.h>
#include <inttypes.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"
#include "stats.h"
#include "utility.h"

struct interfaceStatsStruct {
  uint64_t pktsReceived;
  uint64_t pktsSent;
  uint64_t pktsDropped;
  char rxBytes[32];
  char txBytes[32];
  char rxRate[32];
  char txRate[32];
};
struct deviceStatsStruct {
  uint64_t totalPktsReceived;
  uint64_t totalPktsSent;
  uint64_t totalPktsDropped;
  char totalRxBytes[32];
  char totalTxBytes[32];
  char totalRxRate[32];
  char totalTxRate[32];
};

void cleanup_mongo(void);
void init_mongo_connection(void);

void updateTimeToDB(struct appTimeStruct *appTime);
void updateAppStatsToDB(void);
void updateInterfaceStatsToDB(struct interfaceStatsStruct *interfaceStatsData,
                              uint8_t portid);
void updateDeviceStatsToDB(struct deviceStatsStruct *deviceStatsData);
void insertNetstatToDB(struct netstatStruct *netstatData);

#endif
