#include <cjson/cJSON.h>
#include <nats/nats.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../sa.h" // Include the header file for sa.c to access print_all_sa_rules
#include "sub.h"

extern char ike_string[2][1024];
extern char ike_string_count;

static volatile int running = 1;

static void parse_json_message(const char *json) {
  cJSON *root = cJSON_Parse(json);
  if (!root) {
    printf("Invalid JSON: %s\n", json);
    return;
  }

  cJSON *spi_item = cJSON_GetObjectItemCaseSensitive(root, "SPI");
  cJSON *srcip_item = cJSON_GetObjectItemCaseSensitive(root, "SrcIP");
  cJSON *dstip_item = cJSON_GetObjectItemCaseSensitive(root, "DstIP");

  if (cJSON_IsString(spi_item) && spi_item->valuestring)
    printf("Parsed SPI: %s\n", spi_item->valuestring);
  if (cJSON_IsString(srcip_item) && srcip_item->valuestring)
    printf("Parsed SrcIP: %s\n", srcip_item->valuestring);
  if (cJSON_IsString(dstip_item) && dstip_item->valuestring)
    printf("Parsed DstIP: %s\n", dstip_item->valuestring);

  printf("++++++++++++++++++Atif these are SA Rules++++++++++++++\n");
  print_all_sa_rules();

  cJSON_Delete(root);
}
void onMsg(natsConnection *conn, natsSubscription *sub, natsMsg *msg,
           void *closure) {
  const char *subject = natsMsg_GetSubject(msg);
  const char *data = natsMsg_GetData(msg);
  printf("Received on [%s]: %s\n", subject, data);
  // Copy data into global ike_string safely
  snprintf(ike_string[ike_string_count++], sizeof(ike_string), "%s", data);
  if (ike_string_count == 2)
    ike_string_count = 0;

  natsMsg_Destroy(msg);
}

void handle_sigint(int sig) { running = 0; }

void *subscriber_thread(void *arg) {
  natsConnection *conn = NULL;
  natsOptions *opts = NULL;
  natsSubscription *sub = NULL;
  natsStatus s;
  const char *subject = "foo";

  signal(SIGINT, handle_sigint);

  if ((s = natsOptions_Create(&opts)) != NATS_OK)
    goto cleanup;
  // 1. Change the scheme to tls:// for a secure connection
  if ((s = natsOptions_SetURL(opts, "tls://localhost:4222")) != NATS_OK)
    goto cleanup;

  // 2. Load the CA certificate to verify the server
  if ((s = natsOptions_LoadCATrustedCertificates(
           opts, "/etc/nats/nats-certs/rootCA.pem")) != NATS_OK)
    goto cleanup;

  // 3. Load the client's certificate and private key to present to the server
  if ((s = natsOptions_LoadCertificatesChain(
           opts, "/etc/nats/nats-certs/client-cert.pem",
           "/etc/nats/nats-certs/client-key.pem")) != NATS_OK)
    goto cleanup;

  if ((s = natsOptions_SetToken(opts, "123$%^")) != NATS_OK)
    goto cleanup;
  if ((s = natsConnection_Connect(&conn, opts)) != NATS_OK)
    goto cleanup;
  if ((s = natsConnection_Subscribe(&sub, conn, subject, onMsg, NULL)) !=
      NATS_OK)
    goto cleanup;

  printf("Subscribed to subject '%s' inside pthread. Press Ctrl+C to exit.\n",
         subject);

  while (running) {
    nats_Sleep(100);
  }

cleanup:
  if (s != NATS_OK)
    fprintf(stderr, "NATS error: %s\n", natsStatus_GetText(s));

  natsSubscription_Destroy(sub);
  natsConnection_Destroy(conn);
  natsOptions_Destroy(opts);

  return NULL; // pthread return
}
// void *subscriber_thread(void *arg)
// {
//     natsConnection *conn = NULL;
//     natsOptions *opts = NULL;
//     natsSubscription *sub = NULL;
//     natsStatus s;
//     const char *subject = "foo";

//     signal(SIGINT, handle_sigint);

//     if ((s = natsOptions_Create(&opts)) != NATS_OK)
//         goto cleanup;

//     // Use TLS URL
//     if ((s = natsOptions_SetURL(opts, "tls://localhost:4222")) != NATS_OK)
//         goto cleanup;

//     // Load CA cert - CORRECTED FUNCTION NAME
//     if ((s = natsOptions_LoadCATrustedCertificates(opts,
//     "/etc/nats/nats-certs/ca.crt")) != NATS_OK)
//         goto cleanup;

//     // Load client cert + key
//     if ((s = natsOptions_LoadCertificatesChain(opts,
//                                                "/etc/nats/nats-certs/client.crt",
//                                                "/etc/nats/nats-certs/client.key"))
//                                                != NATS_OK)
//         goto cleanup;

//     // Add Token authentication
//     if ((s = natsOptions_SetToken(opts, "123$%^")) != NATS_OK)
//         goto cleanup;

//     // Connect
//     if ((s = natsConnection_Connect(&conn, opts)) != NATS_OK)
//         goto cleanup;

//     // Subscribe
//     if ((s = natsConnection_Subscribe(&sub, conn, subject, onMsg, NULL)) !=
//     NATS_OK)
//         goto cleanup;

//     printf("Subscribed to subject '%s' with TLS + Token auth. Press Ctrl+C to
//     exit.\n", subject);

//     while (running)
//         nats_Sleep(100);

// cleanup:
//     if (s != NATS_OK)
//         fprintf(stderr, "NATS error: %s\n", natsStatus_GetText(s));

//     natsSubscription_Destroy(sub);
//     natsConnection_Destroy(conn);
//     natsOptions_Destroy(opts);

//     return NULL;
// }
