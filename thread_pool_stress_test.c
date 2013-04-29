#include <thread_pool.h>
#include <stdio.h>

#define THREADS_NUM 8
#define TASKS_NUM 3

void
dummy_work(void * arg) {
    int i;
    int j;
    for(i = 0; i < 200; i++) {
        for(j = 0; j < 90000000; j++)
            ;
        printf("Hello %i\n", *((int *) arg));
        fflush(stdout);
    }
}

int main() {
    ThreadPool * pool = new_thread_pool(THREADS_NUM);
    
    Task * tasks[TASKS_NUM];
    int args[TASKS_NUM];
    int i;
    for(i = 0; i < TASKS_NUM; i++) {
        args[i] = i;
        tasks[i] = new_task(dummy_work, args + i);
    }
    
    for(i = 0; i < 100000; i++)
        execute_and_wait(tasks, TASKS_NUM, pool);
    
    return 0;
}