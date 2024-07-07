#include "thread_pool.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stddef.h>

static void *thread_routine(void *args){
    thread_pool_t *pool = (thread_pool_t *)args;
    while (!pool->closed)
    {
        // wait for the task
        sem_wait(&(pool->task_num));
        // get the task
        pthread_mutex_lock(&(pool->task_list_lock));
        task_t *task = pool->task_list;
        // pool->task_list = task->next;
        pool->task_list = (task) ? task->next : NULL;
        pthread_mutex_unlock(&(pool->task_list_lock));
        if (task == NULL)
        {
            continue;
        }
        
        task->func(task->arg);
        // free() free args fun?
        free(task);
    }
    return NULL;
}

thread_pool_t *thread_pool_init(int thread_num){
    thread_pool_t *new_thread_pool= (thread_pool_t *)malloc(sizeof(thread_pool_t));
    new_thread_pool->thread_num = thread_num;
    new_thread_pool->task_list = NULL;
    new_thread_pool->task_list_rear = NULL;
    new_thread_pool->closed = 0;
    pthread_mutex_init(&(new_thread_pool->task_list_lock), NULL);
    sem_init(&(new_thread_pool->task_num), 0, 0);
    new_thread_pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_num);
    if (new_thread_pool->threads == NULL)
    {
        return NULL;
    }
    
    for (int i = 0; i < thread_num; i++)
    {
        if (pthread_create((new_thread_pool->threads)+i, NULL, &thread_routine, (void *)new_thread_pool))
        {
            printf("pthread_create() failed: \n");
            free(new_thread_pool->threads);
            return NULL;
        }
    }
    return new_thread_pool;
}
int thread_pool_destroy(thread_pool_t *pool){
    pool->closed = 1;
    for (int i = 0; i < pool->thread_num; i++)
    {
        sem_post(&(pool->task_num));
    }
    
    for (int i = 0; i < pool->thread_num; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }
    sem_close(&(pool->task_num));
    free(pool->threads);
    task_t *task = pool->task_list;
    while (task != NULL)
    {
        task_t *temp = task;
        task = task->next;
        free(temp);
    }
    pthread_mutex_destroy(&(pool->task_list_lock));
    sem_destroy(&(pool->task_num));
    free(pool);
    printf("thread pool destroyed\n");
}


int thread_pool_add_task(thread_pool_t *pool, void (*func)(void *arg), void *arg){
    // check if there are too many tasks waiting
    int current_task_num;
    sem_getvalue(&(pool->task_num), &current_task_num);
    if (current_task_num > pool->max_task_num)
    {
        return 1;
    }
    

    task_t *new_task = (task_t *)malloc(sizeof(task_t));
    if (new_task == NULL)
    {
        return 1;
    }
    
    new_task->func = func;
    new_task->arg = arg;
    new_task->next = NULL;
    pthread_mutex_lock(&(pool->task_list_lock));
    if (pool->task_list == NULL)
    {
        pool->task_list = new_task;
        pool->task_list_rear = new_task;
    }
    else
    {
        pool->task_list_rear->next = new_task;
        pool->task_list_rear = new_task;
    }
    pthread_mutex_unlock(&(pool->task_list_lock));
    sem_post(&(pool->task_num));
    return 0;
}