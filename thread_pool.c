#include "thread_pool.h"
#include <stdlib.h>
#include <stdio.h>
#include <sched.h>

//helpers

//called by threads in the threadPool.
void* runTasks(void* ptr){
    Queue *queue = (Queue*)ptr;
    while(1){
        Task *task = (Task*)dequeue(queue);
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
        task->state = TASK_RUNNING;
        void *result = task->run(task->args);
        task->returnResult = result;
        task->state = TASK_COMPLETED;
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_testcancel();
    }
}

//thread_pool.h implementation

ThreadPool *threadPool_create(int nThreads, int capacityOfQueue){
    ThreadPool *pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    pool->isStopped = THREAD_POOL_RUNNING;
    pool->capacityOfTaskQueue = capacityOfQueue;
    pool->taskQueue = newQueue(capacityOfQueue);
    pool->nThreads = nThreads;
    pool->threads = (pthread_t*)malloc(nThreads * sizeof(pthread_t));
    for(int i=0; i<nThreads; i++){
        pthread_create(&pool->threads[i], NULL, runTasks, pool->taskQueue);
    }
    return pool;
}

Task *threadPool_execute(ThreadPool *threadPool, Task *task){
    if(threadPool->isStopped != 0 ){
        task->state = TASK_FAILED;
    }
    else{
        enqueue(threadPool->taskQueue, task);
        task->state = TASK_QUEUED;
    }
    return task;
}

void threadPool_stop(ThreadPool *threadPool){
    printf("stopping threadpool...\n");
    threadPool->isStopped = THREAD_POOL_STOPPING;
    for(int i=0; i<threadPool->nThreads; i++){
        pthread_cancel(threadPool->threads[i]);
    }
    for(int i=0; i<threadPool->nThreads; i++){
        pthread_join(threadPool->threads[i], NULL);
        printf("thread %p stopped!\n", threadPool->threads[i]);
    }
    threadPool->isStopped = THREAD_POOL_STOPPED;
}

int threadPool_destroy(ThreadPool *threadPool){
    if(threadPool->isStopped == THREAD_POOL_RUNNING) return 1;
    while(threadPool->isStopped == THREAD_POOL_STOPPING){
        sched_yield();
    }
    destroyQueue(threadPool->taskQueue);
    free(threadPool->threads);
    free(threadPool);
    return 0;
}

Task *task_create(void* (*run)(void*), void *args){
    Task* task = (Task*)malloc(sizeof(Task));
    task->state = 0;
    task->run = run;
    task->args = args;
    task->returnResult = NULL;
    return task;
}

void task_destroy(Task *task){
    //assume task won't be destroyed while in queue/running
    free(task);
}