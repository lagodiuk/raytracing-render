#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mt_render.h"

#define true    1

#ifdef MT_DEBUG
# define debugf(...) do { printf(__VA_ARGS__); } while (0)
#else
# define debugf(...)
#endif

#define assertf(cond, ...) do { if (cond) { fprintf(stderr, __VA_ARGS__); exit(1); } } while (0)

static void *task_thread(void *arg) {
    mt_worker_t *w = (mt_worker_t *) arg;
    mt_tasks_t *t = w->state;

    int ret = 0;

    while (true) {
        /* wait for start condition: all are busy */
        ret = pthread_mutex_lock(&t->pmutex);
        assertf(ret, "Thread[%d]: pthread_mutex_lock() failed(%d)\n", w->num, ret);

        debugf("Thread[%d]: waiting for start\n", w->num);
        while (t->available_workers > 0) {     // I am lazy, I won't work until all are busy
            ret = pthread_cond_wait(&t->pcond, &t->pmutex);
            assertf(ret, "Thread[%d]: pthread_cond_wait() failed(%d)\n", w->num, ret);
        }

        ret = pthread_mutex_unlock(&t->pmutex);
        assertf(ret, "Thread[%d]: pthread_mutex_unlock() failed(%d)\n", w->num, ret);

        /* do the work */
        ret = w->work(w);
        if (ret) debugf("Thread[%d]: worker returned %d\n", w->num, ret);

        /* we're done */
        ret = pthread_mutex_lock(&t->pmutex);
        assertf(ret, "Thread[%d]: pthread_mutex_lock(ready) failed(%d)\n", w->num, ret);

        debugf("Thread[%d]: done\n", w->num);
        ++ t->available_workers;

        ret = pthread_mutex_unlock(&t->pmutex);
        assertf(ret, "Thread[%d]: pthread_mutex_unlock(ready) failed(%d)\n", w->num, ret);

        ret = pthread_cond_broadcast(&t->pcond);
        assertf(ret, "Thread[%d]: pthread_cond_broadcast(ready) failed(%d)\n", w->num, ret);
    }
}

mt_tasks_t * mt_new_pool(work_func_t *work) {
    int ret, i;
    pthread_attr_t pattr;

    mt_tasks_t *t = (mt_tasks_t *) malloc(sizeof(mt_tasks_t)); 

    t->n_cpu = 32;//sysconf( _SC_NPROCESSORS_ONLN );
    debugf("mt_render_init: %d cores\n", t->n_cpu);

    /* initialize workers and threads */
    t->available_workers = t->n_cpu;
    t->workers = (mt_worker_t *) malloc(sizeof(mt_worker_t) * t->n_cpu);

    pthread_mutex_init(&t->pmutex, NULL);
    pthread_cond_init(&t->pcond, NULL);

    ret = pthread_attr_init(&pattr);
    assertf(ret, "mt_render_init: pthread_attr_init() failed(%d)\n", t->n_cpu);

    for (i = 0; i < t->n_cpu; ++i) {
        mt_worker_t *w = t->workers + i;

        w->state = t;
        w->num = i;
        w->work = work;
        w->data = NULL;

        ret = pthread_create(&(w->pthr), &pattr, &task_thread, w);
        assertf(ret, "mt_render_init: pthread_create() failed(%d) for thread %d\n", ret, i);
    }

    return t;
}

void mt_render_start(mt_tasks_t *t) {
    /* broadcast start condition for workers */
    int ret = 0;
    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "mt_render_start: pthread_mutex_lock(start) failed(%d)\n", ret);

    debugf("mt_render_start: available_workers: %d\n", t->available_workers);
    t->available_workers = 0;      /* make them all busy */

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "mt_render_start: pthread_mutex_unlock(start) failed(%d)\n", ret);

    ret = pthread_cond_broadcast(&t->pcond);
    assertf(ret, "mt_render_start: pthread_cond_broadcast(start) failed(%d)\n", ret);
}

void mt_render_wait(mt_tasks_t *t) {
    int ret;
    /* wait for ready condition */
    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "mt_render_wait: pthread_mutex_lock(ready) failed(%d)\n", ret);

    while (t->available_workers < t->n_cpu) {
        ret = pthread_cond_wait(&t->pcond, &t->pmutex);
        assertf(ret, "mt_render_wait: pthread_cond_wait(ready) failed(%d)\n", ret);
    }

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "mt_render_wait: pthread_mutex_unlock(ready) failed(%d)\n", ret);
}

