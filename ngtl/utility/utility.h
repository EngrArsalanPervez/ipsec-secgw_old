#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <arpa/inet.h>
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
#include <time.h>

void bytesNormalize(uint64_t bytes, char *string);
void dec2IP(uint32_t dec, char *ip);
const char *getDateTimeWithTimezone(void);
void get_date_time();
#endif
