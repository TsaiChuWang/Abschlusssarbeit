#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  ALPHA,
  BETA
} queue_code_t;

typedef struct {
    int* items;
    int front, rear, size;
    int max_buffer;
} circular_queue;

typedef struct {
    circular_queue alpha;
    circular_queue beta;
    int max_buffer;
} link_priority_queue;

// Initialize a circular queue
void init_circular_queue(circular_queue *cqueue, int max_buffer_size) {
  cqueue->items = (int*)malloc(max_buffer_size*sizeof(int));
    memset(cqueue->items, -1, max_buffer_size*sizeof(int));
    cqueue->front = 0;
    cqueue->rear = 0;
    cqueue->size = 0;
    cqueue->max_buffer = max_buffer_size;
}

// Initialize priority queue
void initlink_priority_queue(link_priority_queue *pqueue, int max_buffer_size) {
    init_circular_queue(&pqueue->alpha, max_buffer_size);
    init_circular_queue(&pqueue->beta, max_buffer_size);
    pqueue->max_buffer = max_buffer_size;
}

// Check if a queue is empty
int is_empty(circular_queue *cqueue) {
    return cqueue->size == 0;
}

// Check if a queue is full
int is_full(circular_queue *cqueue) {
    return cqueue->size == cqueue->max_buffer;
}

// Get the total size of both queues
int total_size(link_priority_queue *pqueue) {
    return pqueue->alpha.size + pqueue->beta.size;
}

// Dequeue function
int dequeue(link_priority_queue *pqueue) {
    if (!is_empty(&pqueue->alpha)) {
        int value = pqueue->alpha.items[pqueue->alpha.front];
        pqueue->alpha.front = (pqueue->alpha.front + 1) % pqueue->max_buffer;
        pqueue->alpha.size--;
        return value;
    } else if (!is_empty(&pqueue->beta)) {
        int value = pqueue->beta.items[pqueue->beta.front];
        pqueue->beta.front = (pqueue->beta.front + 1) % pqueue->max_buffer;
        pqueue->beta.size--;
        return value;
    }
    return -1; // Queue is empty
}

// Enqueue function
void enqueue(link_priority_queue *pqueue, queue_code_t code, int value, int *drop_value) {
    *drop_value = -1;

    if (code == BETA) { // Beta queue (low priority)
        if (total_size(pqueue) >= pqueue->max_buffer) {
            *drop_value = value; // Drop the new value
        } else {
            pqueue->beta.items[pqueue->beta.rear] = value;
            pqueue->beta.rear = (pqueue->beta.rear + 1) % pqueue->max_buffer;
            pqueue->beta.size++;
        }
    } else { // Alpha queue (high priority)
        if (is_full(&pqueue->alpha)) {
            *drop_value = value;
            // printf("Queue is full for value %d\n", value);
        } else if (total_size(pqueue) >= pqueue->max_buffer) {
            *drop_value = pqueue->beta.items[pqueue->beta.front]; // Drop from Beta queue
            pqueue->beta.front = (pqueue->beta.front + 1) % pqueue->max_buffer;
            pqueue->beta.size--;

            pqueue->alpha.items[pqueue->alpha.rear] = value;
            pqueue->alpha.rear = (pqueue->alpha.rear + 1) % pqueue->max_buffer;
            pqueue->alpha.size++;
        } else {
            pqueue->alpha.items[pqueue->alpha.rear] = value;
            pqueue->alpha.rear = (pqueue->alpha.rear + 1) % pqueue->max_buffer;
            pqueue->alpha.size++;
        }
    }
}

// Print queue for debugging
void print_queue(link_priority_queue *pqueue) {
    printf("Alpha Queue: ");
    for (int i = 0; i < pqueue->alpha.size; i++)
        printf("%d ", pqueue->alpha.items[(pqueue->alpha.front + i) % pqueue->max_buffer]);
    
    printf("\nBeta Queue: ");
    for (int i = 0; i < pqueue->beta.size; i++)
        printf("%d ", pqueue->beta.items[(pqueue->beta.front + i) % pqueue->max_buffer]);
    
      printf("\ntotal_size = %d\n", total_size(pqueue));
    printf("\n");
}

// Example usage
int main() {
    link_priority_queue pqueue;
    initlink_priority_queue(&pqueue, 10); // Max buffer size

    int drop_value;

    // for(int i=0;i<10;i++){
    //   enqueue(&pqueue, ALPHA, i, &drop_value);
    // }
    // printf("drop = %d \n", drop_value);
    // print_queue(&pqueue);
    // enqueue(&pqueue, ALPHA, 10, &drop_value);
    // printf("drop = %d \n", drop_value);
    // print_queue(&pqueue);

    // printf("Dequeued: %d\n", dequeue(&pqueue));
    // print_queue(&pqueue);
    // enqueue(&pqueue, ALPHA, 10, &drop_value);
    // printf("drop = %d \n", drop_value);
    // print_queue(&pqueue);

    // printf("Dequeued: %d\n", dequeue(&pqueue));
    // print_queue(&pqueue);
    // enqueue(&pqueue, ALPHA, 10, &drop_value);
    // printf("drop = %d \n", drop_value);
    // print_queue(&pqueue);
    
    // print_queue(&pqueue);
    // enqueue(&pqueue, ALPHA, 11, &drop_value);
    // printf("drop = %d \n", drop_value);
    // print_queue(&pqueue);

    
    
    for(int i=0;i<9;i++){
      enqueue(&pqueue, ALPHA, i, &drop_value);
      // printf("total_size = %d\n", total_size(&pqueue));
    }
    printf("drop = %d \n", drop_value);
    print_queue(&pqueue);
    enqueue(&pqueue, BETA, 9, &drop_value);
    printf("drop = %d \n", drop_value);
    print_queue(&pqueue);

    // 
    print_queue(&pqueue);
    enqueue(&pqueue, ALPHA, 10, &drop_value);
    printf("drop = %d \n", drop_value);
    print_queue(&pqueue);

    printf("Dequeued: %d\n", dequeue(&pqueue));
    enqueue(&pqueue, BETA, 9, &drop_value);
    print_queue(&pqueue);
    enqueue(&pqueue, ALPHA, 10, &drop_value);
    print_queue(&pqueue);

    printf("Dequeued: %d\n", dequeue(&pqueue));
    print_queue(&pqueue);
    enqueue(&pqueue, ALPHA, 10, &drop_value);
    printf("drop = %d \n", drop_value);
    print_queue(&pqueue);
    
    print_queue(&pqueue);
    enqueue(&pqueue, ALPHA, 11, &drop_value);
    printf("drop = %d \n", drop_value);
    print_queue(&pqueue);

    // enqueue(&pqueue, BETA, 10, &drop_value);
    // enqueue(&pqueue, BETA, 20, &drop_value);
    // enqueue(&pqueue, ALPHA, 30, &drop_value);
    // enqueue(&pqueue, ALPHA, 40, &drop_value);
    // enqueue(&pqueue, BETA, 50, &drop_value);
    // enqueue(&pqueue, BETA, 60, &drop_value);
    // enqueue(&pqueue, ALPHA, 70, &drop_value);
    // print_queue(&pqueue);
    // enqueue(&pqueue, ALPHA, 80, &drop_value); // This should drop a Beta element

    // print_queue(&pqueue);

    // printf("Dequeued: %d\n", dequeue(&pqueue));
    // printf("Dequeued: %d\n", dequeue(&pqueue));
    
    // print_queue(&pqueue);

    return 0;
}
