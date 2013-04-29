#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <queue.h>
#include <pthread.h>

typedef
struct {
    Queue * tasks;
    pthread_mutex_t tasks_lock;
    pthread_cond_t tasks_cond;
    
    int completed_tasks_num;
    pthread_mutex_t completed_tasks_num_lock;
    pthread_cond_t completed_tasks_num_cond;
    
    pthread_t * threads;
    int threads_num;
}
ThreadPool;

enum
TaskType
{NORMAL, TERMINATE};

typedef
struct {
    enum TaskType type;
    void (* func)(void *);
    void * arg;
}
Task;

ThreadPool *
new_thread_pool(int threads_num);

Task *
new_task(void (* func)(void *), void * arg);

void
execute(Task ** tasks,
        int count,
        ThreadPool * pool);

#endif