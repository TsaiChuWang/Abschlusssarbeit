#define LINK_CAPACITY_QUEUE_H

#ifdef LINK_CAPACITY_QUEUE_H

#define MAX_QUEUE_SIZE 100

typedef struct{
    int packets[MAX_QUEUE_SIZE];

    int front;
    int rear;
} link_capacity_queue;

void initQueue(link_capacity_queue *pqueue)
{
    pqueue->front = -1;
    pqueue->rear = -1;
}

int isFull(link_capacity_queue *pqueue){
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

void record_queue_packets(int* packets, const char* packets_data_path){
    FILE* file = fopen(packets_data_path, "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }
    for (int index = 0;index<MAX_QUEUE_SIZE;index++) {
        fprintf(file, "%d", *(packets+index));
        if (index < MAX_QUEUE_SIZE-1) 
            fprintf(file, ",");
    }
    fclose(file);
}

void record_link_capacity_queue(link_capacity_queue queue, const char* folder_path){
    char data_path[MAX_PATH_LENGTH];

    sprintf(data_path, "%s/link_queue/front.txt", folder_path);
    FILE* link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.front);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue/rear.txt", folder_path);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.rear);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue/packets.csv", folder_path);
    record_queue_packets(queue.packets, data_path);
}

int read_rear_or_front(const char* data_path){
    FILE* file = fopen(data_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE); // Terminate program if file can't be opened
    }

    int value;
    if (fscanf(file, "%d", &value) != 1) { // Try to read an integer
        fprintf(stderr, "Error reading integer from file\n");
        fclose(file);
        exit(EXIT_FAILURE); // Terminate program if no integer is found
    }

    fclose(file);
    return value;
}

int* read_packets_from_csv(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file for reading");
        return NULL;
    }

    char buffer[MAX_BUFFER_SIZE];
    if (!fgets(buffer, sizeof(buffer), file)) {
        perror("Error reading from file");
        fclose(file);
        return NULL;
    }
    fclose(file);

    int* array = (int*)malloc(MAX_QUEUE_SIZE * sizeof(int));
    if (!array) {
        perror("Memory allocation error");
        return NULL;
    }

    char* token = strtok(buffer, ",");
    size_t index = 0;
    while (token) {
        array[index++] = atoi(token);
        token = strtok(NULL, ",");
    }
    return array;
}

void read_link_capacity_queue(link_capacity_queue* pqueue, const char* folder_path){
    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%s/link_queue/front.txt", folder_path);
    pqueue->front = read_rear_or_front(data_path);
    sprintf(data_path, "%s/link_queue/rear.txt", folder_path);
    pqueue->rear = read_rear_or_front(data_path);    
    sprintf(data_path, "%s/link_queue/packets.csv", folder_path);

    int* temp_packets = read_packets_from_csv(data_path);
    for(int index=0;index<MAX_QUEUE_SIZE;index++)
        *(pqueue->packets+index) = *(temp_packets+index);
    
}

void print_link_queue(link_capacity_queue queue){
    printf("front = %d\n", queue.front);
    printf("rear = %d\n", queue.rear);

}
#endif