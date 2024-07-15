#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "semaphore.h"

void initBinarySemaphore(BinarySemaphore *sem, int value) {
    sem->value = value;
    if(pthread_mutex_init(&sem->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    if(pthread_cond_init(&sem->cond, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
}

void waitBinarySemaphore(BinarySemaphore *sem) {
    if(pthread_mutex_lock(&sem->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    while (sem->value <= 0) {
        if(pthread_cond_wait(&sem->cond, &sem->mutex) != 0) {
            perror("pthread_cond_wait");
            exit(1);
        }
    }
    sem->value--;
    if(pthread_mutex_unlock(&sem->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

void signalBinarySemaphore(BinarySemaphore *sem) {
    if(pthread_mutex_lock(&sem->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    sem->value++;
    if(pthread_cond_signal(&sem->cond) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&sem->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

void initCountingSemaphore(CountingSemaphore *sem, int value) {
    sem->sem.value = value;
    if(pthread_mutex_init(&sem->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    if(pthread_cond_init(&sem->cond, NULL) != 0) {
        perror("pthread_cond_init");
        exit(1);
    }
}

void waitCountingSemaphore(CountingSemaphore *sem) {
    if(pthread_mutex_lock(&sem->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    while (sem->sem.value <= 0) {
        if(pthread_cond_wait(&sem->cond, &sem->mutex) != 0) {
            perror("pthread_cond_wait");
            exit(1);
        }
    }
    sem->sem.value--;
    if(pthread_mutex_unlock(&sem->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

void signalCountingSemaphore(CountingSemaphore *sem) {
    if(pthread_mutex_lock(&sem->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
    sem->sem.value++;
    if(pthread_cond_signal(&sem->cond) != 0) {
        perror("pthread_cond_signal");
        exit(1);
    }
    if(pthread_mutex_unlock(&sem->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}
