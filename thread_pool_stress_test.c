#include <thread_pool.h>
#include <stdio.h>
#include <stdlib.h>

#define THREADS_NUM 5
#define TASKS_NUM 7

void
dummy_work(void * arg) {
    int i;
    int j;
    for(i = 0; i < 50; i++) {
        for(j = 0; j < 10000000; j++)
            rand(); // a trick against -O2 optimization of gcc
        printf("Hello %i\n", *((int *) arg));
        fflush(stdout);
    }
}

int main() {
    srand(1);
    
    ThreadPool * pool = new_thread_pool(THREADS_NUM);
    
    Task * tasks[TASKS_NUM];
    int args[TASKS_NUM];
    int i;
    for(i = 0; i < TASKS_NUM; i++) {
        args[i] = i;
        tasks[i] = new_task(dummy_work, args + i);
    }
    
    for(i = 0; i < 10; i++) {
        execute_and_wait(tasks, TASKS_NUM, pool);
        printf("Pool queue size: %i\n", get_size(pool->tasks));
    }
    
    for(i = 0; i < TASKS_NUM; i++) {
        free(tasks[i]);
    }
    
    return 0;
}