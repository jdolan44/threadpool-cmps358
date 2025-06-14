#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "blocking_queue.h"

void *produce(void* ptr){
    printf("pthread %p: begin produce\n", pthread_self());
    Queue *queue = (Queue*)ptr;
    for(int x = 0; x < 5; x++){
        int* num = malloc(sizeof(int));
        *num = x;
        enqueue(queue, num);
        sleep(1);
    }
}

void *consume(void* ptr){
    printf("pthread %p: begin consume\n", pthread_self());
    Queue *queue = (Queue*)ptr;
    for(int x = 0; x < 5; x++){
        int* removed = dequeue(queue);
        free(removed);
        sleep(1);
    }
}

//used at the end of test functions.
void printStatus(char* testName, int status){
    printf("%s test: ", testName);
    if(status){printf("success\n");}
    else{printf("failed\n");}
}

//tests functionality of the size() function.
void runSizeTest(){
    int success = 1;
    Queue *queue = newQueue(5);
    //assert that queue is empty
    success = success & (size(queue) == 0);
    
    char* item = "hello world";
    for(int x = 0; x<3; x++){
        enqueue(queue, item);
    }
    success = success & (size(queue) == 3);

    for(int x = 0; x<3; x++){
        dequeue(queue);
    }
    success = success & (size(queue) == 0);
    
    destroyQueue(queue);

    printStatus("size", success);
}

//test that items are stored and retrieved in the correct order.
void runStorageTest(){
    int success = 1;
    Queue *queue = newQueue(5);
    char* item1 = "my item";
    char* item2 = "my item 2";
    enqueue(queue, item1);
    enqueue(queue, item2);
    success = success & (item1 == dequeue(queue));
    success = success & (item2 == dequeue(queue));
    destroyQueue(queue);

    printStatus("storage", success);
}

//test that the enqueue() function blocks the calling thread.
void runEnqueueBlockingTest(){
    Queue *queue = newQueue(4);
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, produce, queue);
    sleep(10);
    pthread_create(&thread2, NULL, consume, queue);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    int success = (size(queue) == 0);
    destroyQueue(queue);
    
    printStatus("enqueue blocking", success);
}
//test that the dequeue() function blocks the calling thread.
void runDequeueBlockingTest(){
    Queue *queue = newQueue(4);
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, consume, queue);
    sleep(10);
    pthread_create(&thread2, NULL, produce, queue);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    int success = (size(queue) == 0);
    destroyQueue(queue);

    printStatus("dequeue blocking", success);
}

int main(){
    runSizeTest();
    runStorageTest();
    runEnqueueBlockingTest();
    runDequeueBlockingTest();
}