#ifndef __MULTITHREADED_RENDER_H__
#define __MULTITHREADED_RENDER_H__

#include <pthread.h>

struct mt_worker;
struct mt_tasks;

typedef int (*work_func_t)(struct mt_worker *);

typedef struct mt_worker {
    pthread_t pthr;
    int num;
    struct mt_tasks *state;
    
    work_func_t work;
    void *data;
} mt_worker_t;

typedef struct mt_tasks {
    int n_cpu;
    mt_worker_t *workers;

    pthread_mutex_t pmutex;
    pthread_cond_t pcond;
    int available_workers;
} mt_tasks_t;


mt_tasks_t * mt_new_pool(work_func_t *);

void mt_render_start(mt_tasks_t *);

void mt_render_wait(mt_tasks_t *);

#endif // __MULTITHREADED_RENDER_H__
