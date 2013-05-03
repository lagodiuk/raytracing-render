#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>

typedef
struct Elem {
    void * obj;
    struct Elem * volatile prev;
}
Elem;

typedef
struct {
    Elem * volatile head;
    Elem * volatile tail;
    volatile int size;
}
Queue;

static inline Queue *
new_queue();

static inline void
release_queue(Queue * q);

static inline void
add(void * obj,
    Queue * q);

static inline void *
get(Queue * q);

static inline int
get_size(Queue * q);

static inline int
is_empty(Queue * q);

static inline Queue *
new_queue() {
    Queue * q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

static inline void
release_queue(Queue * q) {
    while(q->size) // not empty
        get(q);
    
    free(q);
}

static inline int
get_size(Queue * q) {
    return q->size;
}

static inline int
is_empty(Queue * q) {
    return !q->size;
}

static inline void
add(void * obj,
    Queue * q) {
    
    Elem * e = malloc(sizeof(Elem));
    e->obj = obj;
    e->prev = NULL;
    
    if(q->size)
        q->head->prev = e;
    else
        q->tail = e;
    
    q->head = e;
    
    q->size++;
}

static inline void *
get(Queue * q) {
    
    if(!q->size) // is empty
        return NULL;
    
    Elem * t = q->tail;
    
    void * obj = t->obj;
    q->tail = t->prev;
    
    free(t);
    
    q->size--;
    
    if(!q->size) {
        q->head = NULL;
        q->tail = NULL;
    }
    
    return obj;
}

#endif