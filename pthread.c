#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10

static int buffer[BUFFER_SIZE] = {0};
static int count = 0;

pthread_t consumer,producer;
pthread_cond_t cond_producer,cond_consumer;
pthread_mutex_t mutex;

void* consume(void* param)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == 0) {
            printf("empty buffer, wait producer\n");
            pthread_cond_wait(&cond_consumer,&mutex);
        }
        count--;
        printf("consume a item\n");
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_producer);
    }
    pthread_exit(0);
    return NULL;
}

void* produce(void* param)
{
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count == BUFFER_SIZE) {
            printf("full buffer, wait consumer\n");
            pthread_cond_wait(&cond_producer,&mutex);
        }
        count++;
        printf("produce a item.\n");
        pthread_mutex_unlock(&mutex);
        pthread_cond_signal(&cond_consumer);
    }
    pthread_exit(0);
    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&cond_producer,NULL);
    pthread_cond_init(&cond_consumer,NULL);

    int err = pthread_create(&consumer,NULL,consume,NULL);
    if (err != 0) {
        printf("consumer thread create failed\n");
        exit(1);
    }

    err = pthread_create(&producer,NULL,produce,NULL);
    if (err != 0) {
        printf("producer thread create failed\n");
        exit(1);
    }

    pthread_join(producer,NULL);
    pthread_join(consumer,NULL);

    pthread_cond_destroy(&cond_producer);
    pthread_cond_destroy(&cond_consumer);
    pthread_mutex_destroy(&mutex);

    return 0;
}
