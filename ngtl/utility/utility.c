#include "utility.h"

void bytesNormalize(uint64_t bytes, char *string) {
  if (bytes > 1000000000)
    sprintf(string, "%.2f GB", (float)bytes / 1000000000);
  else if (bytes > 1000000)
    sprintf(string, "%.2f MB", (float)bytes / 1000000);
  else if (bytes > 1000)
    sprintf(string, "%.2f KB", (float)bytes / 1000);
  else
    sprintf(string, "%lu Bytes", bytes);
}

void dec2IP(uint32_t dec, char *ip) {
  struct in_addr ip_addr;
  ip_addr.s_addr = dec;
  strcpy(ip, inet_ntoa(ip_addr));
}

#define TIME_STR_SIZE 100
const char *getDateTimeWithTimezone(void) {
  static char buffer[TIME_STR_SIZE];
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  strftime(buffer, TIME_STR_SIZE, "%Y-%m-%d %H:%M:%S %Z", timeinfo);
  return buffer;
}

void get_date_time(char *dateTime) {
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(dateTime, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900,
          tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}
