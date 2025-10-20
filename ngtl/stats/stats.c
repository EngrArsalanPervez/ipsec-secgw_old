#include "stats.h"

struct appStatsStruct appStatsData[4] = {0};

void printAppStats(void) {
  // Layer2
  printf("eth:\t\t\t%30lu\t\t\t%30lu\n"
         "ethTypeARP:\t\t%30lu\t\t\t%30lu\n"
         "ethTypeVLAN:\t\t%30lu\t\t\t%30lu\n"
         "ethTypeLLDP:\t\t%30lu\t\t\t%30lu\n"
         "ethTypeIPV4:\t\t%30lu\t\t\t%30lu\n"
         "ethTypeIPV6:\t\t%30lu\t\t\t%30lu\n"
         "ethTypeUNKNOWN:\t\t%30lu\t\t\t%30lu\n\n",
         appStatsData[0].eth, appStatsData[1].eth, appStatsData[0].ethTypeARP,
         appStatsData[1].ethTypeARP, appStatsData[0].ethTypeVLAN,
         appStatsData[1].ethTypeVLAN, appStatsData[0].ethTypeLLDP,
         appStatsData[1].ethTypeLLDP, appStatsData[0].ethTypeIPV4,
         appStatsData[1].ethTypeIPV4, appStatsData[0].ethTypeIPV6,
         appStatsData[1].ethTypeIPV6, appStatsData[0].ethTypeUNKNOWN,
         appStatsData[1].ethTypeUNKNOWN);

  // Layer 3
  printf("ipTypeTCP:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeUDP:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeICMP:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeESP:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeIGMP:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeOSPF:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeGRE:\t\t%30lu\t\t\t%30lu\n"
         "ipTypeUNKNOWN:\t\t%30lu\t\t\t%30lu\n\n",
         appStatsData[0].ipTypeTCP, appStatsData[1].ipTypeTCP,
         appStatsData[0].ipTypeUDP, appStatsData[1].ipTypeUDP,
         appStatsData[0].ipTypeICMP, appStatsData[1].ipTypeICMP,
         appStatsData[0].ipTypeESP, appStatsData[1].ipTypeESP,
         appStatsData[0].ipTypeIGMP, appStatsData[1].ipTypeIGMP,
         appStatsData[0].ipTypeOSPF, appStatsData[1].ipTypeOSPF,
         appStatsData[0].ipTypeGRE, appStatsData[1].ipTypeGRE,
         appStatsData[0].ipTypeUNKNOWN, appStatsData[1].ipTypeUNKNOWN);

  // Layer 4 UDP
  printf("udpTypeDNS:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeDHCP:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeNTP:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeSNMP:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeSYSLOG:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeRIP:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeRADIUS:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeX11:\t\t\t%30lu\t\t\t%30lu\n"
         "udpTypeOPENVPN:\t\t%30lu\t\t\t%30lu\n"
         "udpTypeUNKNOWN:\t\t%30lu\t\t\t%30lu\n\n",
         appStatsData[0].udpServicesData.udpTypeDNS,
         appStatsData[1].udpServicesData.udpTypeDNS,
         appStatsData[0].udpServicesData.udpTypeDHCP,
         appStatsData[1].udpServicesData.udpTypeDHCP,
         appStatsData[0].udpServicesData.udpTypeNTP,
         appStatsData[1].udpServicesData.udpTypeNTP,
         appStatsData[0].udpServicesData.udpTypeSNMP,
         appStatsData[1].udpServicesData.udpTypeSNMP,
         appStatsData[0].udpServicesData.udpTypeSYSLOG,
         appStatsData[1].udpServicesData.udpTypeSYSLOG,
         appStatsData[0].udpServicesData.udpTypeRIP,
         appStatsData[1].udpServicesData.udpTypeRIP,
         appStatsData[0].udpServicesData.udpTypeRADIUS,
         appStatsData[1].udpServicesData.udpTypeRADIUS,
         appStatsData[0].udpServicesData.udpTypeX11,
         appStatsData[1].udpServicesData.udpTypeX11,
         appStatsData[0].udpServicesData.udpTypeOPENVPN,
         appStatsData[1].udpServicesData.udpTypeOPENVPN,
         appStatsData[0].udpServicesData.udpTypeUNKNOWN,
         appStatsData[1].udpServicesData.udpTypeUNKNOWN);

  // Layer 4 TCP
  printf("tcpTypeFTP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeSSH:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeTELNET:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeSMTP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeHTTP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeHTTPS:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeIMAPS:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeMSSQL:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeORACLESQL:\t\t%30lu\t\t\t%30lu\n"
         "tcpTypePPTP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeMYSQL:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeRDP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeVNC:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeNETBIOS:\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeBGP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeSIP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeLDAP:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeSOCKS:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeSQUID:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypePOSTGRESQL:\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeREDIS:\t\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeELASTICSEARCH:\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeMONGODB:\t\t%30lu\t\t\t%30lu\n"
         "tcpTypeUNKNOWN:\t\t%30lu\t\t\t%30lu\n\n",
         appStatsData[0].tcpServicesData.tcpTypeFTP,
         appStatsData[1].tcpServicesData.tcpTypeFTP,
         appStatsData[0].tcpServicesData.tcpTypeSSH,
         appStatsData[1].tcpServicesData.tcpTypeSSH,
         appStatsData[0].tcpServicesData.tcpTypeTELNET,
         appStatsData[1].tcpServicesData.tcpTypeTELNET,
         appStatsData[0].tcpServicesData.tcpTypeSMTP,
         appStatsData[1].tcpServicesData.tcpTypeSMTP,
         appStatsData[0].tcpServicesData.tcpTypeHTTP,
         appStatsData[1].tcpServicesData.tcpTypeHTTP,
         appStatsData[0].tcpServicesData.tcpTypeHTTPS,
         appStatsData[1].tcpServicesData.tcpTypeHTTPS,
         appStatsData[0].tcpServicesData.tcpTypeIMAPS,
         appStatsData[1].tcpServicesData.tcpTypeIMAPS,
         appStatsData[0].tcpServicesData.tcpTypeMSSQL,
         appStatsData[1].tcpServicesData.tcpTypeMSSQL,
         appStatsData[0].tcpServicesData.tcpTypeORACLESQL,
         appStatsData[1].tcpServicesData.tcpTypeORACLESQL,
         appStatsData[0].tcpServicesData.tcpTypePPTP,
         appStatsData[1].tcpServicesData.tcpTypePPTP,
         appStatsData[0].tcpServicesData.tcpTypeMYSQL,
         appStatsData[1].tcpServicesData.tcpTypeMYSQL,
         appStatsData[0].tcpServicesData.tcpTypeRDP,
         appStatsData[1].tcpServicesData.tcpTypeRDP,
         appStatsData[0].tcpServicesData.tcpTypeVNC,
         appStatsData[1].tcpServicesData.tcpTypeVNC,
         appStatsData[0].tcpServicesData.tcpTypeNETBIOS,
         appStatsData[1].tcpServicesData.tcpTypeNETBIOS,
         appStatsData[0].tcpServicesData.tcpTypeBGP,
         appStatsData[1].tcpServicesData.tcpTypeBGP,
         appStatsData[0].tcpServicesData.tcpTypeSIP,
         appStatsData[1].tcpServicesData.tcpTypeSIP,
         appStatsData[0].tcpServicesData.tcpTypeLDAP,
         appStatsData[1].tcpServicesData.tcpTypeLDAP,
         appStatsData[0].tcpServicesData.tcpTypeSOCKS,
         appStatsData[1].tcpServicesData.tcpTypeSOCKS,
         appStatsData[0].tcpServicesData.tcpTypeSQUID,
         appStatsData[1].tcpServicesData.tcpTypeSQUID,
         appStatsData[0].tcpServicesData.tcpTypePOSTGRESQL,
         appStatsData[1].tcpServicesData.tcpTypePOSTGRESQL,
         appStatsData[0].tcpServicesData.tcpTypeREDIS,
         appStatsData[1].tcpServicesData.tcpTypeREDIS,
         appStatsData[0].tcpServicesData.tcpTypeELASTICSEARCH,
         appStatsData[1].tcpServicesData.tcpTypeELASTICSEARCH,
         appStatsData[0].tcpServicesData.tcpTypeMONGODB,
         appStatsData[1].tcpServicesData.tcpTypeMONGODB,
         appStatsData[0].tcpServicesData.tcpTypeUNKNOWN,
         appStatsData[1].tcpServicesData.tcpTypeUNKNOWN);
}

void tcpServices(uint16_t port, uint16_t portid) {
  switch (port) {
  case 20:
  case 21:
    appStatsData[portid].tcpServicesData.tcpTypeFTP++;
    break;
  case 22:
    appStatsData[portid].tcpServicesData.tcpTypeSSH++;
    break;
  case 23:
    appStatsData[portid].tcpServicesData.tcpTypeTELNET++;
    break;
  case 25:
    appStatsData[portid].tcpServicesData.tcpTypeSMTP++;
    break;
  case 80:
    appStatsData[portid].tcpServicesData.tcpTypeHTTP++;
    break;
  case 443:
    appStatsData[portid].tcpServicesData.tcpTypeHTTPS++;
    break;
  case 143:
    appStatsData[portid].tcpServicesData.tcpTypeIMAPS++;
    break;
  case 1433:
    appStatsData[portid].tcpServicesData.tcpTypeMSSQL++;
    break;
  case 1521:
    appStatsData[portid].tcpServicesData.tcpTypeORACLESQL++;
    break;
  case 1723:
    appStatsData[portid].tcpServicesData.tcpTypePPTP++;
    break;
  case 3306:
    appStatsData[portid].tcpServicesData.tcpTypeMYSQL++;
    break;
  case 3389:
    appStatsData[portid].tcpServicesData.tcpTypeRDP++;
    break;
  case 5900:
    appStatsData[portid].tcpServicesData.tcpTypeVNC++;
    break;
  case 137:
  case 138:
  case 139:
    appStatsData[portid].tcpServicesData.tcpTypeNETBIOS++;
    break;
  case 161:
  case 179:
    appStatsData[portid].tcpServicesData.tcpTypeBGP++;
    break;
  case 5060:
  case 5061:
    appStatsData[portid].tcpServicesData.tcpTypeSIP++;
    break;
  case 636:
    appStatsData[portid].tcpServicesData.tcpTypeLDAP++;
    break;
  case 1080:
    appStatsData[portid].tcpServicesData.tcpTypeSOCKS++;
    break;
  case 3128:
    appStatsData[portid].tcpServicesData.tcpTypeSQUID++;
    break;
  case 5432:
    appStatsData[portid].tcpServicesData.tcpTypePOSTGRESQL++;
    break;
  case 6379:
    appStatsData[portid].tcpServicesData.tcpTypeREDIS++;
    break;
  case 9200:
    appStatsData[portid].tcpServicesData.tcpTypeELASTICSEARCH++;
    break;
  case 27017:
  case 27018:
  case 27019:
    appStatsData[portid].tcpServicesData.tcpTypeMONGODB++;
    break;
  default:
    appStatsData[portid].tcpServicesData.tcpTypeUNKNOWN++;
    break;
  }
}

void udpServices(uint16_t port, uint16_t portid) {
  switch (port) {
  case 500:
  case 4500:
    appStatsData[portid].udpServicesData.udpTypeIKEv2++;
    break;
  case 53:
    appStatsData[portid].udpServicesData.udpTypeDNS++;
    break;
  case 67:
  case 68:
    appStatsData[portid].udpServicesData.udpTypeDHCP++;
    break;
  case 123:
    appStatsData[portid].udpServicesData.udpTypeNTP++;
    break;
  case 161:
  case 162:
    appStatsData[portid].udpServicesData.udpTypeSNMP++;
    break;
  case 514:
    appStatsData[portid].udpServicesData.udpTypeSYSLOG++;
    break;
  case 520:
    appStatsData[portid].udpServicesData.udpTypeRIP++;
    break;
  case 1645:
  case 1646:
    appStatsData[portid].udpServicesData.udpTypeRADIUS++;
    break;
  case 6000:
  case 6063:
    appStatsData[portid].udpServicesData.udpTypeX11++;
    break;
  case 1194:
    appStatsData[portid].udpServicesData.udpTypeOPENVPN++;
    break;
  default:
    appStatsData[portid].udpServicesData.udpTypeUNKNOWN++;
    break;
  }
}
