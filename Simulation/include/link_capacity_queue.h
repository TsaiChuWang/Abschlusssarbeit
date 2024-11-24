#define LINK_CAPACITY_QUEUE_H

#ifdef LINK_CAPACITY_QUEUE_H

#define MAX_QUEUE_SIZE 100

typedef struct
{
    int packets[MAX_QUEUE_SIZE];

    int front;
    int rear;
} link_capacity_queue;

void initQueue(link_capacity_queue *pqueue)
{
    pqueue->front = -1;
    pqueue->rear = -1;
}

int isFull(link_capacity_queue *pqueue)
{
    return pqueue->rear == MAX_QUEUE_SIZE - 1;
}

// Check if the queue is empty
int isEmpty(link_capacity_queue *pqueue)
{
    return pqueue->front == -1;
}
int enqueue(link_capacity_queue *pqueue)
{
    if (isFull(pqueue))
    {
        // printf("Queue is full. Packet %d is dropped.\n", packet);
        return PACKET_LABEL_OVER_CAPACITY_DROPPED;
    }
    else
    {
        if (pqueue->front == -1)
        {
            pqueue->front = 0; // If it's the first packet
        }
        pqueue->rear++;
        pqueue->packets[pqueue->rear] = 0;
        // printf("Packet %d added to the queue.\n", packet);
        return PACKET_LABEL_ACCEPT;
    }
}

void dequeue(link_capacity_queue *pqueue)
{
    if (isEmpty(pqueue))
    {
        return; // Queue is empty, nothing to transmit
    }
    else
    {
        pqueue->front++;
        if (pqueue->front > pqueue->rear)
        {
            pqueue->front = pqueue->rear = -1; // Reset queue if it's empty
        }
        return;
    }
}

int simulation_link(int label, link_capacity_queue *pqueue)
{
    if (label != PACKET_LABEL_ACCEPT)
        return label;

    int result = enqueue(pqueue);

    if (!isEmpty(pqueue))
    {
        dequeue(pqueue);
    }

    return result;
}
#endif