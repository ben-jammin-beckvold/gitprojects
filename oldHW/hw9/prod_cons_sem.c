//Ben Beckvold
//Computer Systems HW 9
//Producer-Consumer using semaphores directly

// NOTE:  Don't forget that you will compile this using:
//          gcc THIS_FILE -lpthread  # -lpthread means
//           "libpthread.a" (if -static is used), or "libpthread.so" (default)
//          OR THE SHORTER VERSION:  gcc THIS_FILE -pthread

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX 20
#define LOOPS 100 

int buffer[MAX];
int fill_ptr = 0;
int use_ptr  = 0;
int count    = 0;

sem_t empty, fill;
pthread_mutex_t mutex;

void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
}

int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
}

void init()
{
    pthread_mutex_init(&mutex, NULL);
    sem_init(&fill, 1, 0);
    sem_init(&empty, 1, MAX);
}

void *producer(void * arg) 
{
    int i;
    for (i = 0; i < LOOPS; i++) {
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        put(i);
        pthread_mutex_unlock(&mutex);
        sem_post(&fill);
    }
}

void *consumer(void * arg) 
{
    int i;
    for (i = 0; i < LOOPS; i++) 
    {
        sem_wait(&fill);
        pthread_mutex_lock(&mutex);                  
        int tmp = get();
        printf("%d\n", tmp);
        pthread_mutex_unlock(&mutex);
        sem_post(&empty);
    }
}

void main()
{
    init();

    pthread_t p1, p2, p3, p4, c1, c2, c3, c4;
    pthread_create(&p1, NULL, producer, NULL);
    pthread_create(&p2, NULL, producer, NULL);
    pthread_create(&p3, NULL, producer, NULL);
    pthread_create(&p4, NULL, producer, NULL);
    pthread_create(&c1, NULL, consumer, NULL);
    pthread_create(&c2, NULL, consumer, NULL);
    pthread_create(&c3, NULL, consumer, NULL);
    pthread_create(&c4, NULL, consumer, NULL);
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    pthread_join(p3, NULL); 
    pthread_join(p4, NULL); 
    pthread_join(c1, NULL); 
    pthread_join(c2, NULL); 
    pthread_join(c3, NULL); 
    pthread_join(c4, NULL); 
}
