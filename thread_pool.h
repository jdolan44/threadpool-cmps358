#include <pthread.h>
#include "blocking_queue.h"

#define TASK_QUEUED 1
#define TASK_RUNNING 2
#define TASK_COMPLETED 3
#define TASK_FAILED 4

#define THREAD_POOL_RUNNING 0
#define THREAD_POOL_STOPPING 1
#define THREAD_POOL_STOPPED 2

typedef struct THREAD_POOL {
    int isStopped; // 0: false; 1: being stopped; 2; all threads terminated
    int capacityOfTaskQueue; // max # of tasks in the queue at a time
    Queue *taskQueue; 
    int nThreads; // # of threads in the ThreadPool
    pthread_t *threads;
} ThreadPool;

typedef struct TASK {
    int state; // 1: queued, 2: running: 3: completed: 4: failed
    void *(*run)(void *);
    void *args;
    void *returnResult;
} Task;

ThreadPool *threadPool_create(int nThreads, int capacityOfQueue);
Task *threadPool_execute(ThreadPool *threadPool, Task *task);
void threadPool_stop(ThreadPool *threadPool);
int threadPool_destroy(ThreadPool *threadPool);

Task *task_create(void* (*run)(void*), void *args);
void task_destroy(Task *task);