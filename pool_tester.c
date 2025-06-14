#include "thread_pool.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* doThing(void* ptr){
    sleep(1);
    printf("task executed!\n");
}

void* add(void* ptr){
    int* numbers = (int*) ptr;
    int* result = (int*)malloc(sizeof(int));
    *result = numbers[0] + numbers[1];
    return result;
}

void* sub(void* ptr){
    int* numbers = (int*) ptr;
    int* result = (int*)malloc(sizeof(int));
    *result = numbers[0] - numbers[1];
    return result;
}

void printStatus(char* testName, int status){
    printf("%s: ", testName);
    if(status){printf("success\n");}
    else{printf("failed\n");}
}

//test that tasks will return the correct result.
void runTaskReturnTest(){
    int success = 1;
    int* numbers = malloc(sizeof(int)*2);
    numbers[0] = 4;
    numbers[1] = 2;
    ThreadPool *pool = threadPool_create(2, 10);
    Task* addTask = task_create(add, numbers);
    Task* subTask = task_create(sub, numbers);

    threadPool_execute(pool, addTask);
    threadPool_execute(pool, subTask);
    threadPool_stop(pool);
    success = success & (addTask->state == TASK_COMPLETED);
    success = success & (subTask->state == TASK_COMPLETED);
    success = success & (*((int*)addTask->returnResult) == 6);
    success = success & (*((int*)subTask->returnResult) == 2);
    printStatus("tasks return correct result", success);

    free(numbers);
    free(addTask->returnResult);
    task_destroy(addTask);
    free(subTask->returnResult);
    task_destroy(subTask);
    threadPool_destroy(pool);
}

//test that tasks will fail if they are run while the threadpool is stopping.
void runTaskFailTest(){
    int success = 1;
    ThreadPool *pool = threadPool_create(1, 10);
    Task *task = task_create(doThing, NULL);
    threadPool_stop(pool);
    threadPool_execute(pool, task);
    success = success & (task->state == TASK_FAILED);
    printStatus("tasks fail if pool is stopping", success);
    task_destroy(task);
    threadPool_destroy(pool);
}

void runPoolDestroyTest(){
    ThreadPool *pool = threadPool_create(3, 10);
    Task* task = task_create(doThing, NULL);
    
    for(int i=0; i<10; i++){
         threadPool_execute(pool, task);
    }
    sleep(7);
    threadPool_stop(pool);
    threadPool_destroy(pool);
    printf("if you're seeing this, the threadpool didn't deadlock! :) \n");
    task_destroy(task);
}

int main(){    
    runTaskReturnTest();
    runTaskFailTest();
    runPoolDestroyTest();
}