#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include "queue.h"

extern Bounded_Buffer *screenManagerQueue;


UnboundedQueue *createUnboundedQueue() {
    UnboundedQueue *queue = malloc(sizeof(UnboundedQueue));
    if(queue == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }
    queue->head = NULL;
    queue->tail = NULL;
    if(pthread_mutex_init(&queue->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    if(pthread_cond_init(&queue->full, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
    if(pthread_cond_init(&queue->empty, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
    queue->DispatcherDoneEnqueue = 1;
    queue->itemsInQueue = 0;
    return queue;
}

void enqueueUnbounded(UnboundedQueue *queue, Item articale) {
    Node *newNode = malloc(sizeof(Node));
    if(newNode == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }
    newNode->item = articale;
    newNode->next = NULL;

    if(pthread_mutex_lock(&queue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }

    if (queue->tail == NULL) {
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
    if(pthread_cond_signal(&queue->empty) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

Item dequeueUnbounded(UnboundedQueue *queue) {
    if (isEmptyUnbounded(queue)) {
        Item item;
        // empty item means queue is over
        strcpy(item.name, "EMPTY");
        return item;
    }
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    Node *firstNode = queue->head;
    Item item = firstNode->item;
    queue->head = firstNode->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    if(pthread_cond_signal(&queue->full) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    free(firstNode);
    return item;
}
// Check if the queue is empty
int isEmptyUnbounded(UnboundedQueue *queue) {
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    int empty = (queue->head == NULL);
    // printf("Empty: %d\n", empty);
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    return empty;
}
// This function creates a queue with a given size
Queue *createQueue(int size) {
    Queue *queue = malloc(sizeof(Queue));
    if (queue == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }
    queue->buffer = malloc(size * sizeof(int));
    if(queue->buffer == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }
    // Initialize the queue
    queue->size = size;
    queue->front = 0;
    queue->last = -1;
    queue->count = 0;
    queue->newsCounter = 0;
    queue->sportsCounter = 0;
    queue->weatherCounter = 0;
    queue->doneEnqueue = 1;
    queue->doneDequeue = 1;
    queue->itemsInQueue = 0;
    if(pthread_mutex_init(&queue->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    if(pthread_cond_init(&queue->full, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
    if(pthread_cond_init(&queue->empty, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
    return queue;
}

void enqueue(Queue *queue, int item) {
    while (isFull(queue)) {}
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    queue->last = (queue->last + 1) % queue->size;
    queue->buffer[queue->last] = item;
    queue->count++;
    if(pthread_cond_signal(&queue->empty) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

int dequeue(Queue *queue) {
    if (isEmpty(queue)) {
        return -1;
    }
    if(pthread_mutex_lock(&queue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    int item = queue->buffer[queue->front];
    queue->front = (queue->front + 1) % queue->size;
    queue->count--;
    if(pthread_cond_signal(&queue->full) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&queue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    return item;
}

int isEmpty(Queue *queue) {
    return (queue->count == 0);
}

int isFull(Queue *queue) {
    return (queue->count == queue->size);
}

Bounded_Buffer *createBoundedBuffer(int size) {
    Bounded_Buffer *queue = malloc(sizeof(Bounded_Buffer));
    if (queue == NULL) {
        perror("Memory allocation failed.\n");
        exit(1);
    }
    queue->buffer = malloc(size * sizeof(char *));
    queue->front = 0;
    queue->last = -1;
    queue->count = 0;
    queue->size = size;
    queue->doneEnqueue = 1;
    queue->doneDequeue = 1;
    queue->isDonePrinting = 0;
    queue->itemsInQueue = 0;
    if(pthread_mutex_init(&queue->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    initCountingSemaphore(&queue->empty, size);
    initCountingSemaphore(&queue->full, 0);
    return queue;
}

void insert(char *s) {
    waitCountingSemaphore(&screenManagerQueue->empty);
    if(pthread_mutex_lock(&screenManagerQueue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    // printf("Inserting %s\n", s);
    screenManagerQueue->last = (screenManagerQueue->last + 1) % screenManagerQueue->size;
    screenManagerQueue->buffer[screenManagerQueue->last] = strdup(s);
    screenManagerQueue->count++;
    if(pthread_mutex_unlock(&screenManagerQueue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    signalCountingSemaphore(&screenManagerQueue->full);
}

char *removeItem() {
    waitCountingSemaphore(&screenManagerQueue->full);
    if(pthread_mutex_lock(&screenManagerQueue->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    char *item = screenManagerQueue->buffer[screenManagerQueue->front];
    screenManagerQueue->front = (screenManagerQueue->front + 1) % screenManagerQueue->size;
    screenManagerQueue->count--;
    if(pthread_mutex_unlock(&screenManagerQueue->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
    signalCountingSemaphore(&screenManagerQueue->empty);
    return item;
}

int isBoundedBufferEmpty() {
    // printf("Count: %d\n", screenManagerQueue->count);
    return (screenManagerQueue->count == 0);
}

int isBoundedBufferFull() {
    return (screenManagerQueue->count == screenManagerQueue->size);
}
