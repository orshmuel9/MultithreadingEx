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
                char str[20];
                int countOfStr;
                int articaleType = dequeue(currentQueue);
                currentQueue->itemsInQueue--;
                if (articaleType == -1) {
                    continue;
                }
                Item newArticale;
                if (articaleType == 1) {
                    countOfStr = currentQueue->sportsCounter;
                    newArticale = createItem(producers[currentProducer].producerNumber, countOfStr, "SPORTS");
                    sleep(0.1); 
                    enqueueUnbounded(sportsQueue, newArticale);
                    sportsQueue->itemsInQueue++;
                    currentQueue->sportsCounter++;
                } else if (articaleType == 2) {
                    countOfStr = currentQueue->newsCounter;
                    newArticale = createItem(producers[currentProducer].producerNumber, countOfStr, "NEWS");
                    sleep(0.1); 
                    enqueueUnbounded(newsQueue, newArticale);
                    newsQueue->itemsInQueue++;
                    currentQueue->newsCounter++;
                } else if (articaleType == 3) {
                    countOfStr = currentQueue->weatherCounter;
                    newArticale = createItem(producers[currentProducer].producerNumber, countOfStr, "WEATHER");
                    sleep(0.1);
                    enqueueUnbounded(weatherQueue, newArticale);
                    weatherQueue->itemsInQueue++;
                    currentQueue->weatherCounter++;
                }
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
    return NULL;
}
