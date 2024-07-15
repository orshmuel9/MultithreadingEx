#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "semaphore.h"
#include "queue.h"
#include "producer.h"
#include "dispatch.h"
#include "coeditor.h"
#include "screen_manager.h"
#include "item.h"

// Global variables
UnboundedQueue *sportsQueue;
UnboundedQueue *weatherQueue;
UnboundedQueue *newsQueue;
Queue *producerQueues;
Bounded_Buffer *screenManagerQueue;
Producer *producers;

pthread_mutex_t coEditorsDoneMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t coEditorsDoneCond = PTHREAD_COND_INITIALIZER;

void readConfigFile(const char *filename, int *numProducers, int *coEditorQueueSize, int *countOfProducts) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open the file.\n");
        exit(1);
    }

    char buffer[100];
    int queueSize, numProducts, producerNumber;
    *numProducers = 0;
    *countOfProducts = 0;

    // Read the file line by line
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (strncmp(buffer, "PRODUCER", 8) == 0) {
            producerNumber = atoi(&buffer[9]);
            if (fgets(buffer, sizeof(buffer), file) != NULL) {
                numProducts = atoi(buffer);
                *countOfProducts += numProducts;
            }
            if (fgets(buffer, sizeof(buffer), file) != NULL) {
                sscanf(buffer, "queue size = %d", &queueSize);
                producers = realloc(producers, (*numProducers + 1) * sizeof(Producer));
                if (producers == NULL) {
                    perror("Memory allocation failed.\n");
                    exit(1);
                }
                producers[*numProducers].numProducts = numProducts;
                producers[*numProducers].queueSize = queueSize;
                producers[*numProducers].producerID = *numProducers;
                producers[*numProducers].producerNumber = producerNumber;
                (*numProducers)++;
            }
        } else if (strncmp(buffer, "Co-Editor queue size =", 22) == 0) {
            sscanf(buffer, "Co-Editor queue size = %d", coEditorQueueSize);
        }
    }

    fclose(file);
}

void initializeQueues(int numProducers, int coEditorQueueSize) {
    producerQueues = malloc(numProducers * sizeof(Queue));
    if (producerQueues == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }

    for (int i = 0; i < numProducers; i++) {
        producerQueues[i] = *createQueue(producers[i].queueSize);
    }

    sportsQueue = createUnboundedQueue();
    newsQueue = createUnboundedQueue();
    weatherQueue = createUnboundedQueue();
    screenManagerQueue = createBoundedBuffer(coEditorQueueSize);
}

void createThreads(int numProducers, int countOfProducts, pthread_t *producerThreads, pthread_t *dispatcherThread, pthread_t *coEditorThreads, pthread_t *screenManagerThread) {
    for (int i = 0; i < numProducers; i++) {
        if (pthread_create(&producerThreads[i], NULL, produce, &producers[i]) != 0) {
            perror("Failed to create producer thread.\n");
            exit(1);
        }
    }

    if (pthread_create(dispatcherThread, NULL, dispatch, (void *)&numProducers) != 0) {
        perror("Failed to create dispatcher thread.\n");
        exit(1);
    }

    if (pthread_create(&coEditorThreads[0], NULL, coEditor, "SPORTS") != 0) {
        perror("Failed to create co-editor thread.\n");
        exit(1);
    }
    if (pthread_create(&coEditorThreads[1], NULL, coEditor, "NEWS") != 0) {
        perror("Failed to create co-editor thread.\n");
        exit(1);
    }
    if (pthread_create(&coEditorThreads[2], NULL, coEditor, "WEATHER") != 0) {
        perror("Failed to create co-editor thread.\n");
        exit(1);
    }

    if (pthread_create(screenManagerThread, NULL, screenManager, (void *)&countOfProducts) != 0) {
        perror("Failed to create screen manager thread.\n");
        exit(1);
    }
}

void joinThreads(int numProducers, pthread_t *producerThreads, pthread_t dispatcherThread, pthread_t *coEditorThreads, pthread_t screenManagerThread) {
    for (int i = 0; i < numProducers; i++) {
        if (pthread_join(producerThreads[i], NULL) != 0) {
            perror("Failed to join producer thread.\n");
            exit(1);
        }
    }

    if (pthread_join(dispatcherThread, NULL) != 0) {
        perror("Failed to join dispatcher thread.\n");
        exit(1);
    }

    for (int i = 0; i < 3; i++) {
        if (pthread_join(coEditorThreads[i], NULL) != 0) {
            perror("Failed to join co-editor thread.\n");
            exit(1);
        }
    }

    if (pthread_join(screenManagerThread, NULL) != 0) {
        perror("Failed to join screen manager thread.\n");
        exit(1);
    }
}

void freeMemory(int numProducers) {
    free(producers);
    free(sportsQueue);
    free(newsQueue);
    free(weatherQueue);
    for (int i = 0; i < numProducers; i++) {
        free(producerQueues[i].buffer);
    }
    free(producerQueues);
    free(screenManagerQueue->buffer);
    free(screenManagerQueue);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Please provide the file name as a command-line argument.\n");
        return 1;
    }

    int numProducers, coEditorQueueSize, countOfProducts;
    readConfigFile(argv[1], &numProducers, &coEditorQueueSize, &countOfProducts);

    initializeQueues(numProducers, coEditorQueueSize);

    pthread_t producerThreads[numProducers];
    pthread_t dispatcherThread;
    pthread_t coEditorThreads[3];
    pthread_t screenManagerThread;

    createThreads(numProducers, countOfProducts, producerThreads, &dispatcherThread, coEditorThreads, &screenManagerThread);
    joinThreads(numProducers, producerThreads, dispatcherThread, coEditorThreads, screenManagerThread);
    freeMemory(numProducers);

    return 0;
}
