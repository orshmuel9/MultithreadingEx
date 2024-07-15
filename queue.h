#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include "semaphore.h"
#include "item.h"  // Include item.h

typedef struct Node {
    Item item;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
    int itemsInQueue;
    int DispatcherDoneEnqueue;
} UnboundedQueue;

UnboundedQueue *createUnboundedQueue();
void enqueueUnbounded(UnboundedQueue *queue, Item item);
Item dequeueUnbounded(UnboundedQueue *queue);
int isEmptyUnbounded(UnboundedQueue *queue);

typedef struct {
    int *buffer;
    int front;
    int last;
    int count;
    int size;
    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
    int newsCounter;
    int sportsCounter;
    int weatherCounter;
    int doneEnqueue;
    int doneDequeue;
    int itemsInQueue;
} Queue;

Queue *createQueue(int size);
void enqueue(Queue *queue, int item);
int dequeue(Queue *queue);
int isEmpty(Queue *queue);
int isFull(Queue *queue);

typedef struct {
    char **buffer;
    int front;
    int last;
    int count;
    int size;
    pthread_mutex_t mutex;
    CountingSemaphore empty;
    CountingSemaphore full;
    int doneEnqueue;
    int doneDequeue;
    int isDonePrinting;
    int itemsInQueue;
} Bounded_Buffer;

Bounded_Buffer *createBoundedBuffer(int size);
void insert(char *s);
char *removeItem();
int isBoundedBufferEmpty();
int isBoundedBufferFull();

#endif
