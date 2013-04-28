#ifndef __QUEUE_H__
#define __QUEUE_H__

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

Queue *
new_queue();

void
release_queue(Queue * q);

int
get_size(Queue * q);

void
add(void * obj,
    Queue * q);

void *
get(Queue * q);

#endif