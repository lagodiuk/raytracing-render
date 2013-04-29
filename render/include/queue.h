#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>

typedef
struct Elem {
    void * obj;
    struct Elem * prev;
}
Elem;

typedef
struct {
    Elem * head;
    Elem * tail;
    int size;
}
Queue;

static inline Queue *
new_queue() {
    Queue * q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

static inline void *
get(Queue * q);

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
    return q->size == 0;
}

static inline void
add(void * obj,
    Queue * q) {
    
    Elem * e = malloc(sizeof(Elem));
    e->obj = obj;
    e->prev = NULL;
    
    if(q->head)
        q->head->prev = e;
    q->head = e;
    
    if(!q->tail)
        q->tail = e;
    
    q->size++;
}

static inline void *
get(Queue * q) {
    if(is_empty(q))
        return NULL;
    
    void * obj = q->tail->obj;
    Elem * to_remove = q->tail;
    q->tail = to_remove->prev;
    free(to_remove);
    
    q->size--;
    return obj;
}

#endif