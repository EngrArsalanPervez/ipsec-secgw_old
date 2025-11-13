#ifndef _STATS_H_
#define _STATS_H_

#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <sys/types.h>

struct appTimeStruct {
    uint64_t totalSecondsElapsed;
    uint8_t sec;
    uint8_t min;
    uint8_t hrs;
    uint8_t day;
    uint8_t mon;
    uint8_t year;
    uint8_t time[32];
};

struct tcpServicesStruct {
    uint64_t tcpTypeFTP;
    uint64_t tcpTypeSSH;
    uint64_t tcpTypeTELNET;
    uint64_t tcpTypeSMTP;
    uint64_t tcpTypeHTTP;
    uint64_t tcpTypeHTTPS;
    uint64_t tcpTypeIMAPS;
    uint64_t tcpTypeMSSQL;
    uint64_t tcpTypeORACLESQL;
    uint64_t tcpTypePPTP;
    uint64_t tcpTypeMYSQL;
    uint64_t tcpTypeRDP;
    uint64_t tcpTypeVNC;
    uint64_t tcpTypeNETBIOS;
    uint64_t tcpTypeBGP;
    uint64_t tcpTypeSIP;
    uint64_t tcpTypeLDAP;
    uint64_t tcpTypeSOCKS;
    uint64_t tcpTypeSQUID;
    uint64_t tcpTypePOSTGRESQL;
    uint64_t tcpTypeREDIS;
    uint64_t tcpTypeELASTICSEARCH;
    uint64_t tcpTypeMONGODB;
    uint64_t tcpTypeUNKNOWN;
};

struct udpServicesStruct {
    uint64_t udpTypeIKEv2;
    uint64_t udpTypeDNS;
    uint64_t udpTypeDHCP;
    uint64_t udpTypeNTP;
    uint64_t udpTypeSNMP;
    uint64_t udpTypeSYSLOG;
    uint64_t udpTypeRIP;
    uint64_t udpTypeRADIUS;
    uint64_t udpTypeX11;
    uint64_t udpTypeOPENVPN;
    uint64_t udpTypeUNKNOWN;
};

struct appStatsStruct {
    // Layer2
    uint64_t eth;
    uint64_t ethTypeARP;
    uint64_t ethTypeVLAN;
    uint64_t ethTypeIPV4;
    uint64_t ethTypeIPV6;
    uint64_t ethTypeLLDP;
    uint64_t ethTypeUNKNOWN;

    // Layer3
    uint64_t ipTypeTCP;
    uint64_t ipTypeUDP;
    uint64_t ipTypeICMP;
    uint64_t ipTypeESP;
    uint64_t ipTypeIGMP;
    uint64_t ipTypeOSPF;
    uint64_t ipTypeGRE;
    uint64_t ipTypeUNKNOWN;

    // Layer4
    struct tcpServicesStruct tcpServicesData;
    struct udpServicesStruct udpServicesData;
};

extern struct appStatsStruct appStatsData[4];

void printAppStats(void);
void tcpServices(uint16_t port, uint16_t portid);
void udpServices(uint16_t port, uint16_t portid);

#endif
