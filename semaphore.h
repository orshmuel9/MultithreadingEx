#ifndef SEMAPHORE_H
#define SEMAPHORE_H

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} BinarySemaphore;

void initBinarySemaphore(BinarySemaphore *sem, int value);
void waitBinarySemaphore(BinarySemaphore *sem);
void signalBinarySemaphore(BinarySemaphore *sem);

typedef struct {
    BinarySemaphore sem;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} CountingSemaphore;

void initCountingSemaphore(CountingSemaphore *sem, int value);
void waitCountingSemaphore(CountingSemaphore *sem);
void signalCountingSemaphore(CountingSemaphore *sem);

#endif
