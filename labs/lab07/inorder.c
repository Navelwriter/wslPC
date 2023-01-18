// This problem refers to the file inorder.c. 
// In this system, there are threads numbered 1 through 4 and there is a critical section that the threads are trying to enter. 
// The threads are required to enter the critical section in order (e.g., 1 enters the critical section first, 2 following that, then 3, and finally 4).
// If a thread n finishes and thread n+1 is waiting, then thread n+1 immediately executes.
// However, if thread n+1 is not waiting, no threads should be allowed into the critical section until the correct thread arrives in order. 
// For example, if thread 1 finishes and thread 2 is not yet waiting to enter the critical section, then no other thread should be allowed in until thread 2 comes in and finishes.

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
int waiting = 0;
int next = 1;


void *thread(void *arg)
{
        int *num = (int *)arg;
        printf("%d wants to enter the critical section\n", *num);
        pthread_mutex_lock(&lock);
        while(*num != next) {
                waiting = *num;
                pthread_cond_wait(&cond, &lock); //wait for the next thread to finish
        }
        next++;
        if(waiting == next)
                pthread_cond_signal(&cond); //signal the next thread to start
        pthread_mutex_unlock(&lock);
        printf("%d is finished with the critical section\n", *num);

        return NULL;
}

int
main(int argc, char **argv)
{
        pthread_t threads[4];
        int i;
        int nums[] = {2, 1, 4, 3};

        for(i = 0; i < 4; ++i) {
                pthread_create(&threads[i], NULL, thread, &nums[i]);

                if(i == 2)
                        sleep(3);
        }

        for(i = 0; i < 4; ++i) {
                pthread_join(threads[i], NULL);
        }

        printf("Everything finished\n");

        return 0;
}