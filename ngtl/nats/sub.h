#ifndef SUB_H
#define SUB_H

extern volatile int nats_running;

void *subscriber_thread(void *arg);

#endif
