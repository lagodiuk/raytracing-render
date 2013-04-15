#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "mt_render.h"

#if MT_LOG
# define logf(...) do { printf(__VA_ARGS__); } while (0)
# if MT_DEBUG
#  define debugf(...) do { printf(__VA_ARGS__); } while (0)
# else
#  define debugf(...)
# endif
#else
# define logf(...)
#endif

#define assertf(cond, ...) do { if (cond) { fprintf(stderr, __VA_ARGS__); exit(1); } } while (0)

static void broadcast_start(mt_tasks_t *t) {
    /* broadcast start condition for workers */
    int ret = 0;

    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "mt_render_start: pthread_mutex_lock(start) failed(%d)\n", ret);
    debugf("mt_render_start: after mutex_lock()\n");

    t->start_flag = true;
    t->available_workers = 0;

    ret = pthread_cond_broadcast(&t->pcond);
    assertf(ret, "mt_render_start: pthread_cond_broadcast(start) failed(%d)\n", ret);
    debugf("mt_render_start: after cond_broadcast()\n");

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "mt_render_start: pthread_mutex_unlock(start) failed(%d)\n", ret);
    debugf("mt_render_start: after mutex_unlock()\n");
}

static void wait_for_start(mt_worker_t *w) {
    int ret;
    mt_tasks_t *t = w->state;

    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "task_thread[%d]: pthread_mutex_lock() failed(%d)\n", w->num, ret);

    debugf("task_thread[%d]: waiting for start\n", w->num);
    while (! t->start_flag) {
        debugf("task_thread[%d]: before cond_wait(start)\n", w->num);
        ret = pthread_cond_wait(&t->pcond, &t->pmutex);
        assertf(ret, "task_thread[%d]: pthread_cond_wait() failed(%d)\n", w->num, ret);
        debugf("task_thread[%d]: after cond_wait(start)\n", w->num);
    }
    debugf("Thread[%d]: started\n", w->num);

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "Thread[%d]: pthread_mutex_unlock() failed(%d)\n", w->num, ret);
    debugf("task_thread[%d]: after mutex_unlock(start)\n", w->num);
}

static void broadcast_done(mt_worker_t *w) {
    int ret;
    mt_tasks_t *t = w->state;

    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "Thread[%d]: pthread_mutex_lock(ready) failed(%d)\n", w->num, ret);

    debugf("Thread[%d]: done\n", w->num);
    ++ t->available_workers;

    if (t->available_workers == t->n_cpu) {
        ret = pthread_cond_broadcast(&t->pcond);
        assertf(ret, "Thread[%d]: pthread_cond_broadcast(ready) failed(%d)\n", w->num, ret);
        debugf("task_thread[%d]: after cond_broadcast(ready)\n", w->num);
    }

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "Thread[%d]: pthread_mutex_unlock(ready) failed(%d)\n", w->num, ret);
}

static void wait_for_done(mt_tasks_t *t, mt_worker_t *w) {
    int ret;
#   define SIGN_SIZE   128
    char signit[SIGN_SIZE];
    if (w)  snprintf(signit, SIGN_SIZE, "task_thread[%d]", w->num);
    else    strncpy(signit, "mt_render_wait", SIGN_SIZE);

    ret = pthread_mutex_lock(&t->pmutex);
    assertf(ret, "%s: pthread_mutex_lock() failed(%d)\n", signit,  ret);
    debugf("%s: after mutex_lock(all_ready)\n", signit);

    while (t->available_workers < t->n_cpu) {
        ret = pthread_cond_wait(&t->pcond, &t->pmutex);
        assertf(ret, "%s: pthread_cond_wait(all_finished) failed(%d)\n", signit, ret);
        debugf("%s: after cond_wait(all_finished)\n", signit);
    }
    t->start_flag  = false;

    ret = pthread_mutex_unlock(&t->pmutex);
    assertf(ret, "%s: pthread_mutex_unlock() failed(%d)\n", signit, ret);
    debugf("%s: after mutex_unlock(all_ready)\n", signit);
}

static void *task_thread(void *arg) {
    mt_worker_t *w = (mt_worker_t *) arg;
    mt_tasks_t *t = w->state;

    int ret = 0;

    while (1) {
        wait_for_start(w);

        /* do the work */
        ret = w->work(w);
        if (ret) {
            logf("Thread[%d]: worker returned %d\n", w->num, ret);
        }

        broadcast_done(w);

        wait_for_done(t, w);
    }

    return 0;
}

mt_tasks_t * mt_new_pool(work_func_t work) {
    int ret, i;
    pthread_attr_t pattr;

    mt_tasks_t *t = (mt_tasks_t *) malloc(sizeof(mt_tasks_t));

#ifndef N_WORKERS
    t->n_cpu = 2 * sysconf( _SC_NPROCESSORS_ONLN );
#else
    t->n_cpu = N_WORKERS;
#endif
    logf("mt_render_init: %d cores\n", t->n_cpu);

    /* initialize workers and threads */
    t->available_workers = t->n_cpu;
    t->workers = (mt_worker_t *) malloc(sizeof(mt_worker_t) * t->n_cpu);
    t->start_flag = false;

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
        debugf("mt_render_init: thread %d started\n", i);
    }

    return t;
}

void mt_render_start(mt_tasks_t *t) {
    broadcast_start(t);
}

void mt_render_wait(mt_tasks_t *t) {
    wait_for_done(t, NULL);
}

