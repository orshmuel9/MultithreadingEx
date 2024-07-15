#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "queue.h"
#include "producer.h"
#include "item.h" 

extern Queue *producerQueues;
extern Producer *producers;
extern UnboundedQueue *sportsQueue;
extern UnboundedQueue *newsQueue;
extern UnboundedQueue *weatherQueue;

void *dispatch(void *arg) {
    int numProducers = *(int *)arg;
    int countOfDoneQueues = 0;
    int currentProducer = 0;
    while (1) {
        Queue *currentQueue = &producerQueues[currentProducer];
        if (currentQueue->doneDequeue == 1) {
            if (!isEmpty(currentQueue)) {
                int countOfStr;
                int articaleType = dequeue(currentQueue);
                currentQueue->itemsInQueue--;
                if (articaleType == -1) {
                    continue;
                }

                Item newItem;
                if (articaleType == 1) {
                    countOfStr = currentQueue->sportsCounter;
                    newItem = createItem(producers[currentProducer].producerNumber, countOfStr, "SPORTS");
                    enqueueUnbounded(sportsQueue, newItem);
                    sportsQueue->itemsInQueue++;
                    currentQueue->sportsCounter++;
                } else if (articaleType == 2) {
                    countOfStr = currentQueue->newsCounter;
                    newItem = createItem(producers[currentProducer].producerNumber, countOfStr, "NEWS");
                    enqueueUnbounded(newsQueue, newItem);
                    newsQueue->itemsInQueue++;
                    currentQueue->newsCounter++;
                } else if (articaleType == 3) {
                    countOfStr = currentQueue->weatherCounter;
                    newItem = createItem(producers[currentProducer].producerNumber, countOfStr, "WEATHER");
                    enqueueUnbounded(weatherQueue, newItem);
                    weatherQueue->itemsInQueue++;
                    currentQueue->weatherCounter++;
                }
                usleep(100000);
            } else if (currentQueue->doneEnqueue == 0 && currentQueue->itemsInQueue == 0 && isEmpty(currentQueue)) {
                countOfDoneQueues++;
                currentQueue->doneDequeue = 0;
            }
            if (countOfDoneQueues == numProducers) {
                weatherQueue->DispatcherDoneEnqueue = 0;
                newsQueue->DispatcherDoneEnqueue = 0;
                sportsQueue->DispatcherDoneEnqueue = 0;
                return NULL;
            }
        }
        currentProducer = (currentProducer + 1) % numProducers;
    }
    //return NULL;
}
