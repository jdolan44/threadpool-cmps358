#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "blocking_queue.h"

//helpers

int isEmpty(Queue queue){return queue.size == 0;}

int isFull(Queue queue){return queue.size == queue.maxSize;}

Node* newNode(void* item){
    Node *node = (Node*)malloc(sizeof(Node));
    node->item = item;
    node->next = NULL;
    return node;
}

void queue_cleanup(void* ptr){
    pthread_mutex_t* mutex = (pthread_mutex_t*)ptr;
    pthread_mutex_unlock(mutex);
}

//blocking_queue.h implementation

Queue* newQueue(int maxSize){
    Queue *queue = (Queue*)malloc(sizeof(Queue));
    queue->first = NULL;
    queue->last = NULL;
    queue->size = 0;
    queue->maxSize = maxSize;
    pthread_mutex_init(&(queue->lock), NULL);
    pthread_cond_init(&(queue->queueFull), NULL);
    pthread_cond_init(&(queue->queueEmpty), NULL);
    return queue;
}

void enqueue(Queue *queue, void *item){
    pthread_mutex_lock(&(queue->lock));
    Node* node = newNode(item);
    while(isFull(*queue)){
        //printf("pthread %p: waiting to enqueue\n", pthread_self());
        pthread_cond_wait(&(queue->queueFull), &(queue->lock));
    }
    if(isEmpty(*queue)){
        queue->first = node;
        queue->last = node;
    }
    else{
        queue->last->next = node;
        queue->last = node;
    }
    queue->size++;
    //printf("pthread %p: enqueued item %p\n", pthread_self(), item);
    pthread_cond_signal(&(queue->queueEmpty));
    pthread_mutex_unlock(&(queue->lock));
}

void *dequeue(Queue *queue){
    pthread_mutex_lock(&(queue->lock));
    pthread_cleanup_push(queue_cleanup, &(queue->lock));
    while(isEmpty(*queue)){
        printf("pthread %p: waiting to dequeue\n", pthread_self());
        pthread_cond_wait(&(queue->queueEmpty), &(queue->lock));
    }
    pthread_cleanup_pop(0);
    Node* removed = queue->first;
    queue->first = queue->first->next;
    void* item = removed->item;
    free(removed);
    queue->size--;
    //printf("pthread %p: dequeued item %p\n", pthread_self(), item);
    pthread_cond_signal(&(queue->queueFull));
    pthread_mutex_unlock(&(queue->lock));
    return item;
}

int size(Queue *queue){
    pthread_mutex_lock(&(queue->lock));
    int size = queue->size;
    pthread_mutex_unlock(&(queue->lock));
    return size;
}

void destroyQueue(Queue *queue){
    pthread_mutex_destroy(&(queue->lock));
    pthread_cond_destroy(&(queue->queueFull));
    pthread_cond_destroy(&(queue->queueEmpty));
    free(queue);
}