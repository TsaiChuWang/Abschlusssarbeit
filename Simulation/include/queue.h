#define QUEUE_H

#ifdef QUEUE_H

typedef struct{
  int max_queue_size;
  int front;
  int rear;

  long dequeue_interval;
  long dequeue_timestamp;
} queue;

void initQueue(queue *pqueue, int max_queue_size, const configuration config, double bandwidth){
  pqueue->max_queue_size = max_queue_size;
  pqueue->front = -1;
  pqueue->rear = -1;

  long unit = obtainUnit(config);
  pqueue->dequeue_interval = (long)(config.packet_size*(double)ONE_SECOND_IN_NS/(bandwidth*unit));
  pqueue->dequeue_timestamp = 0;
}

int isFull(queue *pqueue){
    return pqueue->rear == pqueue->max_queue_size-1;
}

int isEmpty(queue *pqueue){
    return pqueue->front == -1;
}

int enqueue(queue *pqueue){
    if (isFull(pqueue)){
        // printf("Queue is full. front = %d, rear = %d.\n", pqueue->front, pqueue->rear);
        return PACKET_LABEL_OVER_CAPACITY_DROPPED;
    }else{
        if (pqueue->front == -1){
            pqueue->front = 0; // If it's the first packet
        }
        pqueue->rear++;
        // printf("Packet %d added to the queue.\n", packet);
        return PACKET_LABEL_ACCEPT;
    }
}

void dequeue(queue *pqueue){
    if (isEmpty(pqueue)){
        return; // Queue is empty, nothing to transmit
    }else{
        pqueue->front++;
        // printf("dequeue front = %d")
        if (pqueue->front > pqueue->rear){
            pqueue->front = -1; // Reset queue if it's empty
            pqueue->rear = -1;
        }
        return;
    }
}

void record_queue(queue queue, const char* folder_path){
    char data_path[MAX_PATH_LENGTH];

    sprintf(data_path, "%s/link_queue/max_queue_size.txt", folder_path);
    FILE* link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.max_queue_size);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue/front.txt", folder_path);
    link_file = fopen(data_path, "w");
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

    sprintf(data_path, "%s/link_queue/dequeue_interval.txt", folder_path);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%ld\n", queue.dequeue_interval);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue/dequeue_timestamp.txt", folder_path);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%ld\n", queue.dequeue_timestamp);  
    fclose(link_file);
}

int read_queue_integer_filed(const char* data_path){
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

long read_queue_long_filed(const char* data_path){
    FILE* file = fopen(data_path, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE); // Terminate program if file can't be opened
    }

    long value;
    if (fscanf(file, "%ld", &value) != 1) { // Try to read an integer
        fprintf(stderr, "Error reading integer from file\n");
        fclose(file);
        exit(EXIT_FAILURE); // Terminate program if no integer is found
    }

    fclose(file);
    return value;
}

void read_queue(queue* pqueue, const char* folder_path){
    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%s/link_queue/max_queue_size.txt", folder_path);
    pqueue->max_queue_size = read_queue_integer_filed(data_path);
    sprintf(data_path, "%s/link_queue/front.txt", folder_path);
    pqueue->front = read_queue_integer_filed(data_path);
    sprintf(data_path, "%s/link_queue/rear.txt", folder_path);
    pqueue->rear = read_queue_integer_filed(data_path);    
    sprintf(data_path, "%s/link_queue/dequeue_interval.txt", folder_path);
    pqueue->dequeue_interval = read_queue_long_filed(data_path); 
    sprintf(data_path, "%s/link_queue/dequeue_timestamp.txt", folder_path);
    pqueue->dequeue_timestamp = read_queue_long_filed(data_path); 
}

void print_link_queue(queue queue){
    printf("max_queue_size    = %-d\n", queue.max_queue_size);
    printf("front             = %-d\n", queue.front);
    printf("rear              = %-d\n", queue.rear);
    printf("dequeue_interval  = %-ld\n", queue.dequeue_interval);
    printf("dequeue_timestamp = %-ld\n", queue.dequeue_timestamp);
}
#endif