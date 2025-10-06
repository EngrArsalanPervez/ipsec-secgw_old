#ifndef _LOGS_H_
#define _LOGS_H_

#include <stdio.h>
#include <stdlib.h>

struct Log {
  int data;
  struct Log *next;
};

void push(struct Log **head, int newData);
int pop(struct Log **head);
void printList(struct Log *log);

#endif
