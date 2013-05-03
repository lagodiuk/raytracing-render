#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <queue.h>
#include <pthread.h>

typedef
struct {
    Queue * tasks;
    pthread_mutex_t tasks_lock;
    pthread_cond_t tasks_cond;
    
    pthread_t * threads;
    int threads_num;
}
ThreadPool;

enum
TaskType
{NORMAL, TERMINATE};

enum
TaskStatus
{ACTIVE, DONE};

typedef
struct {
    enum TaskType type;
    
    void (* func)(void *);
    void * arg;
    
    volatile enum TaskStatus status;
    pthread_mutex_t status_lock;
    pthread_cond_t status_cond;
}
Task;

inline static int
get_threads_num(ThreadPool * pool) {
    return pool->threads_num;
}

ThreadPool *
new_thread_pool(int threads_num);

Task *
new_task(void (* func)(void *), void * arg);

void
destroy_task(Task * task);

void
execute_task(Task * task,
             ThreadPool * pool);

void
wait_for_task(Task * task);

void
execute_tasks(Task ** tasks,
              int count,
              ThreadPool * pool);

void
wait_for_tasks(Task ** tasks,
               int count);

#endif