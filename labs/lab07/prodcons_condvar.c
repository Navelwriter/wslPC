#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFERSIZE 5
// When the buffer is empty, the consumers should be blocked. 
// Alternatively, when the the buffer is full, the producers should be blocked.

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int buffer[BUFFERSIZE];
int last_valid_index;

void *
producer(void *arg)
{
        int i;
        int value = *((int*) arg);
        for(i = 0; i < 10; ++i) {
                pthread_mutex_lock(&lock);
                while(last_valid_index == BUFFERSIZE - 1) {
                        printf("Buffer is full, waiting for consumer to eat...\n");
                        pthread_cond_wait(&cond, &lock);
                }
                buffer[last_valid_index + 1] = value;
                last_valid_index++;
                printf("Produced value %d, stored at %d\n", value, last_valid_index);
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&cond);
                value += 1;
        }
        return NULL;
}

void *
consumer(void *arg)
{
        int value, i;
        for(i = 0; i < 10; ++i) {
                pthread_mutex_lock(&lock);
                sleep(1);
                while(last_valid_index == -1) {
                        printf("Buffer is empty, waiting for producer to produce...\n");
                        pthread_cond_wait(&cond, &lock);
                }
                value = buffer[last_valid_index];
                last_valid_index--;
                printf("Consumed value %d, stored at %d\n", value, last_valid_index+1);
                pthread_cond_signal(&cond);
                pthread_mutex_unlock(&lock);
        }
        return NULL;
}

int
main(int argc, char **argv)
{
        pthread_t p1, p2, c1, c2;
        int p1start = 100;
        int p2start = 200;

        last_valid_index = -1; // initially, there is no valid data
        pthread_create(&p1, NULL, producer, &p1start);
        pthread_create(&p2, NULL, producer, &p2start);
        pthread_create(&c1, NULL, consumer, NULL);
        pthread_create(&c2, NULL, consumer, NULL);

        pthread_join(p1, NULL);
        pthread_join(p2, NULL);
        pthread_join(c1, NULL);
        pthread_join(c2, NULL);

        printf("Everything finished...\n");
}