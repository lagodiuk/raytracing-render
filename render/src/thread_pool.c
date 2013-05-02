#include <stdlib.h>
#include <pthread.h>

#include <stdio.h>

#include <queue.h>
#include <thread_pool.h>

void *
worker_thread_loop(void * arg);

ThreadPool *
new_thread_pool(int threads_num) {
    
    ThreadPool * pool = malloc(sizeof(ThreadPool));
    
    pool->tasks = new_queue();
    
    pthread_mutex_init(&pool->tasks_lock, NULL);    
    pthread_cond_init(&pool->tasks_cond, NULL);
    
    pool->threads = calloc(threads_num, sizeof(pthread_t));
    pool->threads_num = threads_num;
    
    int i;
    for(i = 0; i < threads_num; i++) {
        pthread_create(&pool->threads[i], NULL, worker_thread_loop, pool);
    }
    
    return pool;
}

void *
worker_thread_loop(void * arg) {
    ThreadPool * pool = (ThreadPool *) arg;
    
    while(1) {
        pthread_mutex_lock(&pool->tasks_lock);
        
        while(is_empty(pool->tasks))
            pthread_cond_wait(&pool->tasks_cond, &pool->tasks_lock);
        
        Task * task = (Task *) get(pool->tasks);
        
        pthread_mutex_unlock(&pool->tasks_lock);
        
        if(task->type == TERMINATE) {
            break;
        }
        
        task->func(task->arg);
        
        pthread_mutex_lock(&task->status_lock);
        task->status = DONE;
        pthread_mutex_unlock(&task->status_lock);
        pthread_cond_signal(&task->status_cond);
    }
    
    return NULL;
}

Task *
new_task(void (* func)(void *), void * arg) {
    Task * task = malloc(sizeof(Task));
    task->type = NORMAL;
    task->func = func;
    task->arg = arg;
    
    pthread_mutex_init(&task->status_lock, NULL);
    pthread_cond_init(&task->status_cond, NULL);
    
    return task;
}

void
destroy_task(Task * task) {
    pthread_mutex_destroy(&task->status_lock);
    pthread_cond_destroy(&task->status_cond);
    free(task);
}

void
execute(Task * task,
        ThreadPool * pool) {
    
    pthread_mutex_lock(&pool->tasks_lock);
    
    task->status = ACTIVE;
    add(task, pool->tasks);
    
    pthread_mutex_unlock(&pool->tasks_lock);
    pthread_cond_signal(&pool->tasks_cond);
}

void
wait_for_task(Task * task) {
    pthread_mutex_lock(&task->status_lock);
    while(task->status != DONE)
        pthread_cond_wait(&task->status_cond, &task->status_lock);
    pthread_mutex_unlock(&task->status_lock);
}

void
execute_tasks(Task ** tasks,
              int count,
              ThreadPool * pool) {
    
    pthread_mutex_lock(&pool->tasks_lock);
    
    int i;
    for(i = 0; i < count; i++) {
        tasks[i]->status = ACTIVE;
        add(tasks[i], pool->tasks);
    }
    
    pthread_mutex_unlock(&pool->tasks_lock);
    pthread_cond_broadcast(&pool->tasks_cond);
}

void
wait_for_tasks(Task ** tasks,
               int count) {
    int i;
    for(i = 0; i < count; i++) {
        wait_for_task(tasks[i]);
    }
}
