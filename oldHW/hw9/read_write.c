//Ben Beckvold
//Computer Systems HW9
//Reader-Writer using newly defined condition variables

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

#define LOOPS 50

typedef struct condition_var_t
{
    sem_t sem_waiters;
    int waiters;
} condition_var_t;

Pthread_cond_t user;
pthread_mutex_t mutex, cond_mutex;

void cond_var_init(condition_var_t *cond_t)
{
    sem_init(&(cond_t->sem_waiters), 0, 0);
    cond_t->waiters = 0;
}

void condition_var_wait(condition_var_t *cond_t, pthread_mutex_t *lock)
{
    pthread_mutex_unlock(lock);

    pthread_mutex_lock(&cond_mutex);
    cond_t->waiters++;
    pthread_mutex_unlock(&cond_mutex);

    sem_wait(&(cond_t->sem_waiters));

    pthread_mutex_lock(&cond_mutex);
    cond_t->waiters++;
    pthread_mutex_unlock(&cond_mutex);

    pthread_mutex_lock(lock);
}

void condition_var_signal(condition_var_t *cond_t)
{
    pthread_mutex_unlock(&mutex);
    if (cond_t->waiters > 0)
        sem_post(&(cond_t->sem_waiters));
    pthread_mutex_lock(&mutex);
}

int num_writers = 0;
int num_readers = 0;

void init()
{
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&cond_mutex, NULL);
    cond_var_init(&user);
}

void *writer(void * arg) {
    int i;
    for (i = 0; i < LOOPS; i++)
    {
        pthread_mutex_lock(&mutex);
        if (num_readers > 0 || num_writers > 0)
        {
            Pthread_cond_wait(&user, &mutex);
        } 
        num_writers++;
        printf("WRITING!!\n");
        num_writers--;
        if ((num_readers + num_writers) > 0)
            Pthread_cond_signal(&user);
        pthread_mutex_unlock(&mutex);
    }
}

void *reader(void * arg) {
    int i;
    for (i = 0; i < LOOPS; i++)
    {
        pthread_mutex_lock(&mutex);
        if (num_writers > 0)
        {
            Pthread_cond_wait(&user, &mutex);
        }
        num_readers++;
        printf("READING!!\n");
        num_readers--;
        if ((num_readers + num_writers) > 0) 
            Pthread_cond_signal(&user);
        pthread_mutex_unlock(&mutex);
    }
}

void main()
{
    init();

    pthread_t p1, p2, p3, p4, c1, c2, c3, c4;
    pthread_create(&p1, NULL, writer, NULL);
    pthread_create(&p2, NULL, writer, NULL);
    pthread_create(&p3, NULL, writer, NULL);
    pthread_create(&p4, NULL, writer, NULL);
    pthread_create(&c1, NULL, reader, NULL);
    pthread_create(&c2, NULL, reader, NULL);
    pthread_create(&c3, NULL, reader, NULL);
    pthread_create(&c4, NULL, reader, NULL);
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    pthread_join(p3, NULL); 
    pthread_join(p4, NULL); 
    pthread_join(c1, NULL); 
    pthread_join(c2, NULL); 
    pthread_join(c3, NULL); 
    pthread_join(c4, NULL);
}
