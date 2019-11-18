#include <stdlib.h> // rand()
#include <stdio.h>
#include <unistd.h> // sleep()
#include <semaphore.h>
#include <pthread.h>
#include "buffer.h"

//the buffer
buffer_item buffer[BUFFER_SIZE + 1];
size_t buffer_head, buffer_tail;

//mutex and semaphore
sem_t full, empty;
pthread_mutex_t lock;

int insert_item(buffer_item item) {
    sem_wait(&empty);
    pthread_mutex_lock(&lock);
    buffer[buffer_tail] = item;
    buffer_tail = (buffer_tail + 1) % (BUFFER_SIZE + 1);
    pthread_mutex_unlock(&lock);
    sem_post(&full);
    return 0;
}

int remove_item(buffer_item *item) {
    sem_wait(&full);
    pthread_mutex_lock(&lock);
    *item = buffer[buffer_head];
    buffer_head = (buffer_head + 1) % (BUFFER_SIZE + 1);
    pthread_mutex_unlock(&lock);
    sem_wait(&empty);
    return 0;
}

void *producer(void *param) {
    buffer_item item;
    while (1) {
        sleep(rand() % MAX_SLEEP_TIME);
        item = rand();
        insert_item(item);
        printf("Producer produced %d.\n", item);
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {
        sleep(rand() % MAX_SLEEP_TIME);
        remove_item(&item);
        printf("Consumer consumed %d.\n", item);
    }
}

void init_buffer() {
    pthread_mutex_init(&lock, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
    buffer_head = buffer_tail = 0;
}

void destroy_buffer() {
    pthread_mutex_destroy(&lock);
    sem_destroy(&full);
    sem_destroy(&empty);
}