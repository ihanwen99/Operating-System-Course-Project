/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

// semaphore variable
sem_t taskCnt;

// this represents work that has to be 
// completed by a thread in the pool
typedef struct {
    void (*function)(void *p);
    void *data;
} task;

// the work queue
task worktodo[QUEUE_SIZE];
int curTaskNum;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];
pthread_mutex_t mutex;

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) {
    pthread_mutex_lock(&mutex);
    if (curTaskNum == QUEUE_SIZE) {
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    worktodo[curTaskNum++] = t;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// remove a task from the queue
task *dequeue() {
    pthread_mutex_lock(&mutex);
    if (curTaskNum == 0) {
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    task *curTask = &worktodo[--curTaskNum];
    pthread_mutex_unlock(&mutex);
    return curTask;
}

// the worker thread in the thread pool
void *worker(void *param) {
    // execute the task
    while (TRUE) {
        sem_wait(&taskCnt); // block until there is an available task
        task *t = dequeue();
        if (t == NULL) { continue; }
        execute(t->function, t->data);
    }
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p) {
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p) {
    task tmpTask;

    tmpTask.function = somefunction;
    tmpTask.data = p;

    int result = enqueue(tmpTask);

    if (result) {
        printf("Queue is Full! /n");
        return 1;
    }
    sem_post(&taskCnt);    // suceess -> signal the semaphore
    return 0;

}

// initialize the thread pool
void pool_init(void) {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&taskCnt, 0, 0);
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_create(&bee[i], NULL, worker, NULL);
    }
}

// shutdown the thread pool
void pool_shutdown(void) {
    for (int i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cancel(bee[i]);
        pthread_join(bee[i], NULL);
    }
    sem_destroy(&taskCnt);
    pthread_mutex_destroy(&mutex);
}

