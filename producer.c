#include <stdlib.h>
#include <pthread.h>
#include "producer.h"
#include "queue.h"

extern Queue *producerQueues;
extern Producer *producers;

void *produce(void *arg) {
    Producer *producer = (Producer *)arg;
    int producerNumber = producer->producerID;
    Queue *queue = &producerQueues[producerNumber];
    int productCount = 0;
    int numProducts = producer->numProducts;
    while (productCount < numProducts) {
        // Wait until the queue is not full
        while (isFull(queue)) {}
        int randomNumber = (rand() % 3) + 1;
        enqueue(queue, randomNumber);
        queue->itemsInQueue++;
        productCount++;
    }
    queue->doneEnqueue = 0;
    return NULL;
}
