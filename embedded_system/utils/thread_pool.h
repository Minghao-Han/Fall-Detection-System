#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>

#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H 1

typedef struct task {
    void (*func)(void *arg);
    void *arg;
    struct task *next;
} task_t;

typedef struct thread_pool {
    sem_t task_num;
    int max_task_num;
    pthread_mutex_t task_list_lock;
    task_t *task_list;
    task_t *task_list_rear;
    pthread_t *threads;
    int thread_num;
    uint8_t closed;
} thread_pool_t;

thread_pool_t *thread_pool_init(int thread_num);
int thread_pool_destroy(thread_pool_t *pool);
int thread_pool_add_task(thread_pool_t *pool, void (*func)(void *arg), void *arg);

#endif