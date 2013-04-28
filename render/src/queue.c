#include <stdlib.h>
#include <queue.h>

Queue *
new_queue() {
    Queue * q = malloc(sizeof(Queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

void
release_queue(Queue * q) {
    while(q->size) // not empty
        get(q);
    
    free(q);
}

int
get_size(Queue * q) {
    return q->size;
}

int
is_empty(Queue * q) {
    return !q->size;
}

void
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

void *
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

