#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "queue.h"
#include "item.h" 

extern UnboundedQueue *sportsQueue;
extern UnboundedQueue *newsQueue;
extern UnboundedQueue *weatherQueue;
extern Bounded_Buffer *screenManagerQueue;

void *coEditor(void *arg) {
    char *coEditorType = (char *)arg;
    UnboundedQueue *queueArticles;

    //printf("Starting %s co-editor\n", coEditorType);

    if (strcmp(coEditorType, "SPORTS") == 0) {
        queueArticles = sportsQueue;
    } else if (strcmp(coEditorType, "NEWS") == 0) {
        queueArticles = newsQueue;
    } else if (strcmp(coEditorType, "WEATHER") == 0) {
        queueArticles = weatherQueue;
    } else {
        printf("Invalid co-editor type: %s\n", coEditorType);
        return NULL;
    }

    while (1) {
        //printf("%s co-editor checking exit condition\n", coEditorType);
        if (isEmptyUnbounded(queueArticles) && queueArticles->DispatcherDoneEnqueue == 0 && queueArticles->itemsInQueue == 0) {
            screenManagerQueue->isDonePrinting++;
            return NULL;
            //printf("%s co-editor exit condition met\n", coEditorType);
            //break;
        }

        if (!isEmptyUnbounded(queueArticles)) {
            //printf("%s co-editor dequeuing item\n", coEditorType);
            Item item = dequeueUnbounded(queueArticles);
            queueArticles->itemsInQueue--;

            if (strcmp(item.name, "EMPTY") != 0) {
                //printf("%s co-editor processing item: %s\n", coEditorType, item.name);
                usleep(100000); // sleep for 0.1 seconds
                char itemStr[100];
                snprintf(itemStr, sizeof(itemStr), "Producer %d %s %d", item.currentProducer, item.name, item.countOfStr);
                
                //printf("%s co-editor inserting item into screenManagerQueue\n", coEditorType);
                insert(itemStr);
                screenManagerQueue->itemsInQueue++;
                //printf("%s co-editor finished inserting item\n", coEditorType);
            }
        } else {
            //printf("%s co-editor found empty queue, yielding\n", coEditorType);
            sched_yield(); // Give other threads a chance to run
        }
    }
    //printf("%s co-editor finished\n", coEditorType);
}