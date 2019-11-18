/**
 * Example client program that uses thread pool.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"


int main(int argc, int *argv[]) {
    /* 1 Get parameters on the command line*/
    if (argc != 4) {
        fprintf(stderr, "<Input> SleepSecond numProducer numConsumer\n");
        return 0;
    }
    int SleepSecond = atoi(argv[1]);
    size_t numProducer = atoi(argv[2]), numConsumer = atoi(argv[3]);

    /* 2 Initialize buffer*/
    init_buffer();
    /* 3 Create producer threads*/
    pthread_t *producers = malloc(numProducer * sizeof(pthread_t));
    for (int i = 0; i < numProducer; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }
    /* 4 Create consumer therads*/
    pthread_t *consumers = malloc(numConsumer * sizeof(pthread_t));
    for (int i = 0; i < numConsumer; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }
    /* 5 Sleep*/
    printf("Sleep for %d second(s) before exit.\n", SleepSecond);
    sleep(SleepSecond);
    /* 6 Exit*/
    for (int i = 0; i < numProducer; i++) {
        pthread_cancel(producers[i]);
        pthread_join(producers[i], NULL);
    }
    sleep(3);   // time for consumers to consume the left items
    for (int i = 0; i < numConsumer; i++) {
        pthread_cancel(consumers[i]);
        pthread_join(consumers[i], NULL);
    }

    free(producers);
    free(consumers);
    destroy_buffer();

    return 0;
}
