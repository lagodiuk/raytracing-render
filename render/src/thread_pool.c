#include <stdlib.h>
#include <pthread.h>

#include <stdio.h>

#include <queue.h>
#include <thread_pool.h>

void *
worker_thread_loop(void * arg);

ThreadPool *
new_thread_pool(int threads_num) {
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;
    
    ThreadPool * pool = malloc(sizeof(ThreadPool));
    
    pool->tasks = new_queue();
    pool->tasks_lock = mtx;
    pool->tasks_cond = cnd;
    
    pool->completed_tasks_num = 0;
    pool->completed_tasks_num_lock = mtx;
    pool->completed_tasks_num_cond = cnd;
    
    pool->threads = calloc(threads_num, sizeof(pthread_t));
    pool->threads_num = threads_num;
    
    int i;
    for(i = 0; i < threads_num; i++) {
        pthread_create(&(pool->threads[i]), NULL, worker_thread_loop, pool);
    }
    
    return pool;
}

void *
worker_thread_loop(void * arg) {
    ThreadPool * pool = (ThreadPool *) arg;
    
    while(1) {
        pthread_mutex_lock(&(pool->tasks_lock));
        
        while(is_empty(pool->tasks)) {
            pthread_cond_wait(&(pool->tasks_cond), &(pool->tasks_lock));
            continue;
        }
        
        Task * task = (Task *) get(pool->tasks);
        
        pthread_mutex_unlock(&(pool->tasks_lock));
        
        if(task->type == TERMINATE) {
            break;
        }
        
        task->func(task->arg);
        
        pthread_mutex_lock(&(pool->completed_tasks_num_lock));
        pool->completed_tasks_num++;
        pthread_mutex_unlock(&(pool->completed_tasks_num_lock));
        pthread_cond_signal(&(pool->completed_tasks_num_cond));
    }
    
    return NULL;
}

Task *
new_task(void (* func)(void *), void * arg) {
    Task * task = malloc(sizeof(Task));
    task->type = NORMAL;
    task->func = func;
    task->arg = arg;    
    return task;
}

void
execute_and_wait(Task ** tasks,
                 int count,
                 ThreadPool * pool) {

    int curr_num_completed_tasks;
    pthread_mutex_lock(&(pool->completed_tasks_num_lock));
    curr_num_completed_tasks = pool->completed_tasks_num;
    pthread_mutex_unlock(&(pool->completed_tasks_num_lock));
    
    int expected_num_completed_tasks = curr_num_completed_tasks + count;
    
    int i;
    pthread_mutex_lock(&(pool->tasks_lock));
    for(i = 0; i < count; i++) {
        add(tasks[i], pool->tasks);
    }
    pthread_mutex_unlock(&(pool->tasks_lock));
    pthread_cond_broadcast(&(pool->tasks_cond));
    
    pthread_mutex_lock(&(pool->completed_tasks_num_lock));
    while(pool->completed_tasks_num < expected_num_completed_tasks)
        pthread_cond_wait(&(pool->completed_tasks_num_cond), &(pool->completed_tasks_num_lock));
    pthread_mutex_unlock(&(pool->completed_tasks_num_lock));
}

//------------------------------------------------

void
print_hello(void * arg) {
    int i;
    int j;
    for(i = 0; i < 5; i++) {
        for(j = 0; j < 90000000; j++)
            ;
        printf("Hello %s\n", (char *) arg);
        fflush(stdout);
    }
}

int main() {    
    ThreadPool * pool = new_thread_pool(8);
    
    Task * tasks[3];
    tasks[0] = new_task(print_hello, "World");
    tasks[1] = new_task(print_hello, "POSIX");
    tasks[2] = new_task(print_hello, "Facebook");
    
    execute_and_wait(tasks, 3, pool);
    
    execute_and_wait(tasks, 3, pool);
    
    int i;
    for(i = 0; i < 5; i++)
        execute_and_wait(tasks, 3, pool);
    
    return 0;
}