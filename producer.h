#ifndef PRODUCER_H
#define PRODUCER_H

#include "queue.h"

typedef struct {
    int numProducts;
    int queueSize;
    int producerID;
    int producerNumber;
} Producer;

void *produce(void *arg);

#endif