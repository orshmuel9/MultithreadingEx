#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"

extern Bounded_Buffer *screenManagerQueue;

void *screenManager(void *arg) {
    int countOfProducts = *(int *)arg;
    while (1) {
        if ((isBoundedBufferEmpty() && screenManagerQueue->isDonePrinting == 3 && screenManagerQueue->itemsInQueue == 0) || countOfProducts == 0) {
            printf("DONE\n");
            //exit(0);
            return NULL;
            //break;
        }
        if (!isBoundedBufferEmpty()) {
            char *itemStr = removeItem();
            screenManagerQueue->itemsInQueue--;
            if (itemStr == NULL || strcmp(itemStr, "EMPTY") == 0) {
                continue;
            }
            printf("%s\n", itemStr);
            free(itemStr);
            countOfProducts--;
        }
    }
}
