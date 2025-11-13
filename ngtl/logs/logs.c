#include "logs.h"

void push(struct Log **head, int newData)
{
    struct Log *newLog = (struct Log *)malloc(sizeof(struct Log));
    if (!newLog) {
        return;
    }
    newLog->data = newData;
    newLog->next = NULL;

    if (*head == NULL) {
        *head = newLog;
        return;
    }

    struct Log *temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newLog;
}

int pop(struct Log **head)
{
    if (*head == NULL) {
        return -1;
    }
    struct Log *temp = *head;
    int poppedData = temp->data;
    *head = temp->next;
    free(temp);
    return poppedData;
}

void printList(struct Log *log)
{
    while (log != NULL) {
        printf("%d -> ", log->data);
        log = log->next;
    }
    printf("NULL\n");
}
