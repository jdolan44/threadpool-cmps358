#include <pthread.h>

typedef struct QUEUE_NODE {
    void *item;
    struct QUEUE_NODE *next;
} Node;

typedef struct BLOCKING_QUEUE {
    Node *first;
    Node *last;
    int size;
    int maxSize;
    pthread_mutex_t lock;
    pthread_cond_t queueFull;
    pthread_cond_t queueEmpty;
} Queue;

Queue* newQueue();
void enqueue(Queue *queue, void *item);
void *dequeue(Queue *queue);
int size(Queue *queue);
void destroyQueue(Queue *queue);