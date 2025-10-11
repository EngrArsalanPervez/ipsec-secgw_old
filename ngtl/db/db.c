#include "db.h"

// DB Collections
#define DB_TOTALCOLLECTIONS 6
#define PORT_STATS 0
#define INTERFACE_STATS 1
#define DEVICE_STATS 2
#define NET_STATS 3
#define UP_TIME 4

mongoc_client_t *client;
mongoc_collection_t *collection[DB_TOTALCOLLECTIONS];
mongoc_uri_t *uri;

void cleanup_mongo(void) {
  for (uint8_t i = 0; i < DB_TOTALCOLLECTIONS; i++)
    mongoc_collection_destroy(collection[i]);

  mongoc_client_destroy(client);
  mongoc_uri_destroy(uri);
  mongoc_cleanup();
}

void init_mongo_connection(void) {
  mongoc_init();
  uri = mongoc_uri_new("mongodb://localhost:27017");
  if (!uri) {
    fprintf(stderr, "Failed to create MongoDB URI.\n");
    exit(EXIT_FAILURE);
  }

  client = mongoc_client_new_from_uri(uri);
  if (!client) {
    fprintf(stderr, "Failed to connect to MongoDB.\n");
    exit(EXIT_FAILURE);
  }

  for (uint8_t i = 0; i < DB_TOTALCOLLECTIONS; i++) {
    char name[128] = {0};
    switch (i) {
    case PORT_STATS:
      strcpy(name, "portstats");
      break;
    case INTERFACE_STATS:
      strcpy(name, "interfacestats");
      break;
    case DEVICE_STATS:
      strcpy(name, "devicestats");
      break;
    case NET_STATS:
      strcpy(name, "netstats");
      break;
    case UP_TIME:
      strcpy(name, "mainconfigs");
      break;
    default:
      printf("Unknown Collection Name\n");
      exit(1);
    }

    collection[i] = mongoc_client_get_collection(client, "IPSec", name);
    if (!collection[i]) {
      fprintf(stderr, "Failed to access MongoDB collection.\n");
      exit(EXIT_FAILURE);
    }
  }
}

void updateTimeToDB(struct appTimeStruct *appTime) {
  bson_error_t error;
  bson_t *update = NULL;
  bson_t *query = NULL;
  query = BCON_NEW("id", BCON_INT32(1));
  update =
      BCON_NEW("$set", "{", "upInt", BCON_DOUBLE(appTime->totalSecondsElapsed),
               "upTime", BCON_UTF8(appTime->time), "}");
  if (!mongoc_collection_update_one(collection[UP_TIME], query, update, NULL,
                                    NULL, &error)) {
    fprintf(stderr, "%s\n", error.message);
    goto fail;
  }
fail:
  if (query)
    bson_destroy(query);
  if (update)
    bson_destroy(update);

  return;
}

void updateAppStatsToDB(void) {
  for (uint8_t portid = 0; portid < 2; portid++) {
    bson_error_t error;
    bson_t *update = NULL;
    bson_t *query = NULL;
    query = BCON_NEW("portid", BCON_INT32(portid));

    update = BCON_NEW(
        "$set", "{", "eth", BCON_DOUBLE(appStatsData[portid].eth), "ethTypeARP",
        BCON_DOUBLE(appStatsData[portid].ethTypeARP), "ethTypeVLAN",
        BCON_DOUBLE(appStatsData[portid].ethTypeVLAN), "ethTypeIPV4",
        BCON_DOUBLE(appStatsData[portid].ethTypeIPV4), "ethTypeIPV6",
        BCON_DOUBLE(appStatsData[portid].ethTypeIPV6), "ethTypeLLDP",
        BCON_DOUBLE(appStatsData[portid].ethTypeLLDP), "ethTypeUNKNOWN",
        BCON_DOUBLE(appStatsData[portid].ethTypeUNKNOWN), "ipTypeTCP",
        BCON_DOUBLE(appStatsData[portid].ipTypeTCP), "ipTypeUDP",
        BCON_DOUBLE(appStatsData[portid].ipTypeUDP), "ipTypeICMP",
        BCON_DOUBLE(appStatsData[portid].ipTypeICMP), "ipTypeESP",
        BCON_DOUBLE(appStatsData[portid].ipTypeESP), "ipTypeIGMP",
        BCON_DOUBLE(appStatsData[portid].ipTypeIGMP), "ipTypeOSPF",
        BCON_DOUBLE(appStatsData[portid].ipTypeOSPF), "ipTypeGRE",
        BCON_DOUBLE(appStatsData[portid].ipTypeGRE), "ipTypeUNKNOWN",
        BCON_DOUBLE(appStatsData[portid].ipTypeUNKNOWN), "tcpTypeFTP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeFTP),
        "tcpTypeSSH",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeSSH),
        "tcpTypeTELNET",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeTELNET),
        "tcpTypeSMTP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeSMTP),
        "tcpTypeHTTP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeHTTP),
        "tcpTypeHTTPS",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeHTTPS),
        "tcpTypeIMAPS",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeIMAPS),
        "tcpTypeMSSQL",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeMSSQL),
        "tcpTypeORACLESQL",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeORACLESQL),
        "tcpTypePPTP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypePPTP),
        "tcpTypeMYSQL",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeMYSQL),
        "tcpTypeRDP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeRDP),
        "tcpTypeVNC",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeVNC),
        "tcpTypeNETBIOS",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeNETBIOS),
        "tcpTypeBGP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeBGP),
        "tcpTypeSIP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeSIP),
        "tcpTypeLDAP",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeLDAP),
        "tcpTypeSOCKS",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeSOCKS),
        "tcpTypeSQUID",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeSQUID),
        "tcpTypePOSTGRESQL",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypePOSTGRESQL),
        "tcpTypeREDIS",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeREDIS),
        "tcpTypeELASTICSEARCH",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeELASTICSEARCH),
        "tcpTypeMONGODB",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeMONGODB),
        "tcpTypeUNKNOWN",
        BCON_DOUBLE(appStatsData[portid].tcpServicesData.tcpTypeUNKNOWN),
        "udpTypeDNS",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeDNS),
        "udpTypeDHCP",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeDHCP),
        "udpTypeNTP",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeNTP),
        "udpTypeSNMP",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeSNMP),
        "udpTypeSYSLOG",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeSYSLOG),
        "udpTypeRIP",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeRIP),
        "udpTypeRADIUS",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeRADIUS),
        "udpTypeX11",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeX11),
        "udpTypeOPENVPN",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeOPENVPN),
        "udpTypeUNKNOWN",
        BCON_DOUBLE(appStatsData[portid].udpServicesData.udpTypeUNKNOWN), "}");

    if (!mongoc_collection_update_one(collection[PORT_STATS], query, update,
                                      NULL, NULL, &error)) {
      fprintf(stderr, "%s\n", error.message);
      goto fail;
    }
  fail:
    if (query)
      bson_destroy(query);
    if (update)
      bson_destroy(update);
  }
}

void updateInterfaceStatsToDB(struct interfaceStatsStruct *interfaceStatsData,
                              uint8_t portid) {
  bson_error_t error;
  bson_t *update = NULL;
  bson_t *query = NULL;
  query = BCON_NEW("portid", BCON_INT32(portid));

  update = BCON_NEW("$set", "{", "pktsReceived",
                    BCON_DOUBLE(interfaceStatsData->pktsReceived), "pktsSent",
                    BCON_DOUBLE(interfaceStatsData->pktsSent), "pktsDropped",
                    BCON_DOUBLE(interfaceStatsData->pktsDropped), "rxBytes",
                    BCON_UTF8(interfaceStatsData->rxBytes), "txBytes",
                    BCON_UTF8(interfaceStatsData->txBytes), "rxRate",
                    BCON_UTF8(interfaceStatsData->rxRate), "txRate",
                    BCON_UTF8(interfaceStatsData->txRate), "}");

  if (!mongoc_collection_update_one(collection[INTERFACE_STATS], query, update,
                                    NULL, NULL, &error)) {
    fprintf(stderr, "%s\n", error.message);
    goto fail;
  }
fail:
  if (query)
    bson_destroy(query);
  if (update)
    bson_destroy(update);
}

void updateDeviceStatsToDB(struct deviceStatsStruct *deviceStatsData) {
  bson_error_t error;
  bson_t *update = NULL;
  bson_t *query = NULL;
  query = BCON_NEW("deviceid", BCON_INT32(1));

  update =
      BCON_NEW("$set", "{", "totalPktsReceived",
               BCON_DOUBLE(deviceStatsData->totalPktsReceived), "totalPktsSent",
               BCON_DOUBLE(deviceStatsData->totalPktsSent), "totalPktsDropped",
               BCON_DOUBLE(deviceStatsData->totalPktsDropped), "totalRxBytes",
               BCON_UTF8(deviceStatsData->totalRxBytes), "totalTxBytes",
               BCON_UTF8(deviceStatsData->totalTxBytes), "totalRxRate",
               BCON_UTF8(deviceStatsData->totalRxRate), "totalTxRate",
               BCON_UTF8(deviceStatsData->totalTxRate), "}");

  if (!mongoc_collection_update_one(collection[DEVICE_STATS], query, update,
                                    NULL, NULL, &error)) {
    fprintf(stderr, "%s\n", error.message);
    goto fail;
  }
fail:
  if (query)
    bson_destroy(query);
  if (update)
    bson_destroy(update);
}

void insertNetstatToDB(struct netstatStruct *netstatData) {
  bson_error_t error;
  bson_oid_t oid;
  bson_t *doc = bson_new();
  bson_oid_init(&oid, NULL);
  BSON_APPEND_OID(doc, "_id", &oid);

  char srcIP[32] = {0};
  char dstIP[32] = {0};

  dec2IP(netstatData->srcIP, srcIP);
  dec2IP(netstatData->dstIP, dstIP);

  BSON_APPEND_UTF8(doc, "srcIP", srcIP);
  BSON_APPEND_UTF8(doc, "dstIP", dstIP);
  BSON_APPEND_INT32(doc, "srcPort", rte_cpu_to_be_16(netstatData->srcPort));
  BSON_APPEND_INT32(doc, "dstPort", rte_cpu_to_be_16(netstatData->dstPort));
  BSON_APPEND_INT32(doc, "proto", netstatData->proto);
  BSON_APPEND_INT32(doc, "inPort", netstatData->inPort);
  BSON_APPEND_INT32(doc, "outPort", netstatData->outPort);

  char time[64] = {0};
  get_date_time(time);
  BSON_APPEND_UTF8(doc, "dateTime", time);

  if (!doc) {
    fprintf(stderr, "Failed to create BSON document.\n");
    return;
  }

  if (!mongoc_collection_insert_one(collection[NET_STATS], doc, NULL, NULL,
                                    &error))
    fprintf(stderr, "Insert failed: %s\n", error.message);

  bson_destroy(doc);
}
