//Ben Beckvold
//Computer Systems HW9
//Reader-Writer using semaphones as condition variables

// NOTE:  Don't forget that you will compile this using:
//          gcc THIS_FILE -lpthread  # -lpthread means
//           "libpthread.a" (if -static is used), or "libpthread.so" (default)
//          OR THE SHORTER VERSION:  gcc THIS_FILE -pthread

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define Pthread_cond_t condition_var_t
#define Pthread_cond_signal condition_var_signal
#define Pthread_cond_wait condition_var_wait

#define MAX 20
#define LOOPS 100

typedef struct condition_var_t
{
    sem_t sem_waiters;
    int waiters;
} condition_var_t;

Pthread_cond_t empty, fill;
pthread_mutex_t mutex, cond_mutex;

void cond_var_init(condition_var_t *cond_t)
{
    sem_init(&(cond_t->sem_waiters), 0, 0);
    cond_t->waiters = 0;
}

void condition_var_wait(condition_var_t *cond_t, pthread_mutex_t *lock)
{
    pthread_mutex_unlock(lock);
    
    //add 1 to waiters
    pthread_mutex_lock(&cond_mutex);
    cond_t->waiters++;
    pthread_mutex_unlock(&cond_mutex);
    
    sem_wait(&(cond_t->sem_waiters));
   
    //sub 1 from waiters
    pthread_mutex_lock(&cond_mutex);
    cond_t->waiters++;
    pthread_mutex_unlock(&cond_mutex);
    
    pthread_mutex_lock(lock);
}

void condition_var_signal(condition_var_t *cond_t)
{
    pthread_mutex_unlock(&mutex);
    //post if someone is waiting
    if (cond_t->waiters > 0)
        sem_post(&(cond_t->sem_waiters));
    pthread_mutex_lock(&mutex);
}

int buffer[MAX];
int fill_ptr = 0;
int use_ptr  = 0;
int count    = 0;

void init()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&cond_mutex, NULL);
    cond_var_init(&fill);
    cond_var_init(&empty);
}

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

void *producer(void * arg) {
    int i;
    for (i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&mutex);
        while (count == MAX)
            Pthread_cond_wait(&empty, &mutex);
        put(i);
        Pthread_cond_signal(&fill);
        pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void * arg) {
    int i;
    for (i = 0; i < LOOPS; i++) {
        pthread_mutex_lock(&mutex);
        while (count == 0)
            Pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        Pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
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
