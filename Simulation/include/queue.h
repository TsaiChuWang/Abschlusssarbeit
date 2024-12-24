#define QUEUE_H

#ifdef QUEUE_H

typedef struct{
  int max_queue_size;
  int front;
  int rear;

  TIME_TYPE dequeue_interval;
  TIME_TYPE dequeue_timestamp;

  int drop_code;
} queue;

void initQueue(queue *pqueue, int max_queue_size, const configuration config, double bandwidth, int drop_code){
  pqueue->max_queue_size = max_queue_size;
  pqueue->front = -1;
  pqueue->rear = -1;

  long unit =config.unit;
  
  pqueue->dequeue_interval = (TIME_TYPE)(config.packet_size*(double)ONE_SECOND_IN_NS/(bandwidth*unit));
//   printf("dequeue_interval  = %f\n", pqueue->dequeue_interval);
  pqueue->dequeue_timestamp = 0;

  pqueue->drop_code = drop_code;
}

queue* initQueues(int max_queue_size, const configuration config, double bandwidth, int drop_code, int tenant_number){
    queue* queues = (queue*)malloc(sizeof(queue)*tenant_number);
    for(int tenant = 0;tenant<tenant_number;tenant++)
        initQueue((queue*)(queues+tenant), max_queue_size, config, bandwidth, drop_code);
    return queues;
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
        return pqueue->drop_code;
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

    sprintf(data_path, "%s/link_queue_%d/max_queue_size.txt", folder_path, queue.drop_code);
    FILE* link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.max_queue_size);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue_%d/front.txt", folder_path, queue.drop_code);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.front);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue_%d/rear.txt", folder_path, queue.drop_code);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%d\n", queue.rear);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue_%d/dequeue_interval.txt", folder_path, queue.drop_code);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%f\n", queue.dequeue_interval);  
    fclose(link_file);

    sprintf(data_path, "%s/link_queue_%d/dequeue_timestamp.txt", folder_path, queue.drop_code);
    link_file = fopen(data_path, "w");
    if (!link_file) {
        printf("Unable to open file  \"%s\" for writing\n", data_path);
        perror("Unable to open file for writing");  
        return;
    }
    fprintf(link_file, "%f\n", queue.dequeue_timestamp);  
    fclose(link_file);
}

void record_queues(queue* queues, const char* folder_path, int tenant_number){
    char data_path[MAX_PATH_LENGTH];
    int drop_code = (*queues).drop_code;

    for(int tenant = 0;tenant<tenant_number;tenant++){
        sprintf(data_path, "%s/link_queue_%d/max_queue_size_%d.txt", folder_path, drop_code, tenant);
        FILE* link_file = fopen(data_path, "w");
        if (!link_file) {
            printf("Unable to open file  \"%s\" for writing\n", data_path);
            perror("Unable to open file for writing");  
            return;
        }
        fprintf(link_file, "%d\n", (queues+tenant)->max_queue_size);  
        fclose(link_file);

        sprintf(data_path, "%s/link_queue_%d/front_%d.txt", folder_path, drop_code, tenant);
        link_file = fopen(data_path, "w");
        if (!link_file) {
            printf("Unable to open file  \"%s\" for writing\n", data_path);
            perror("Unable to open file for writing");  
            return;
        }
        fprintf(link_file, "%d\n", (queues+tenant)->front);  
        fclose(link_file);

        sprintf(data_path, "%s/link_queue_%d/rear_%d.txt", folder_path, drop_code, tenant);
        link_file = fopen(data_path, "w");
        if (!link_file) {
            printf("Unable to open file  \"%s\" for writing\n", data_path);
            perror("Unable to open file for writing");  
            return;
        }
        fprintf(link_file, "%d\n", (queues+tenant)->rear);  
        fclose(link_file);

        sprintf(data_path, "%s/link_queue_%d/dequeue_interval_%d.txt", folder_path, drop_code, tenant);
        link_file = fopen(data_path, "w");
        if (!link_file) {
            printf("Unable to open file  \"%s\" for writing\n", data_path);
            perror("Unable to open file for writing");  
            return;
        }
        fprintf(link_file, "%f\n", (queues+tenant)->dequeue_interval);  
        fclose(link_file);

        sprintf(data_path, "%s/link_queue_%d/dequeue_timestamp_%d.txt", folder_path, drop_code, tenant);
        link_file = fopen(data_path, "w");
        if (!link_file) {
            printf("Unable to open file  \"%s\" for writing\n", data_path);
            perror("Unable to open file for writing");  
            return;
        }
        fprintf(link_file, "%f\n", (queues+tenant)->dequeue_timestamp);  
        fclose(link_file);
    }
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

double read_queue_double_filed(const char* data_path){
    FILE *file;      
    double value;    

    file = fopen(data_path, "r");
    if (file == NULL) {
        printf("Cannot read file %s\n", data_path);
        perror("Error opening file ");  
        return FAILED;  
    }

    if (fscanf(file, "%lf", &value) != 1) {
        printf("Error reading value from file %s\n", data_path);
        perror("Error reading value from file ");  
        fclose(file);  
        return FAILED;   
    }

    fclose(file);  

    return value;  
}

void read_queue(queue* pqueue, const char* folder_path, int drop_code){
    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%s/link_queue_%d/max_queue_size.txt", folder_path, drop_code);
    pqueue->max_queue_size = read_queue_integer_filed(data_path);
    sprintf(data_path, "%s/link_queue_%d/front.txt", folder_path, drop_code);
    pqueue->front = read_queue_integer_filed(data_path);
    sprintf(data_path, "%s/link_queue_%d/rear.txt", folder_path, drop_code);
    pqueue->rear = read_queue_integer_filed(data_path);    
    sprintf(data_path, "%s/link_queue_%d/dequeue_interval.txt", folder_path, drop_code);
    pqueue->dequeue_interval = read_queue_double_filed(data_path); 
    sprintf(data_path, "%s/link_queue_%d/dequeue_timestamp.txt", folder_path, drop_code);
    pqueue->dequeue_timestamp = read_queue_double_filed(data_path); 
    pqueue->drop_code = drop_code;
}

void read_queues(queue* pqueues, const char* folder_path, int drop_code, int tenant_number){
    char data_path[MAX_PATH_LENGTH];
    for(int tenant = 0;tenant<tenant_number;tenant++){
        sprintf(data_path, "%s/link_queue_%d/max_queue_size_%d.txt", folder_path, drop_code, tenant);
        (pqueues+tenant)->max_queue_size = read_queue_integer_filed(data_path);
        sprintf(data_path, "%s/link_queue_%d/front_%d.txt", folder_path, drop_code, tenant);
        (pqueues+tenant)->front = read_queue_integer_filed(data_path);
        sprintf(data_path, "%s/link_queue_%d/rear_%d.txt", folder_path, drop_code, tenant);
        (pqueues+tenant)->rear = read_queue_integer_filed(data_path);    
        sprintf(data_path, "%s/link_queue_%d/dequeue_interval_%d.txt", folder_path, drop_code, tenant);
        (pqueues+tenant)->dequeue_interval = read_queue_double_filed(data_path); 
        sprintf(data_path, "%s/link_queue_%d/dequeue_timestamp_%d.txt", folder_path, drop_code, tenant);
        (pqueues+tenant)->dequeue_timestamp = read_queue_double_filed(data_path); 
        (pqueues+tenant)->drop_code = drop_code;
    }

}

void print_queue(queue queue){
    printf("max_queue_size    = %-d\n", queue.max_queue_size);
    printf("front             = %-d\n", queue.front);
    printf("rear              = %-d\n", queue.rear);
    printf("dequeue_interval  = %-f\n", queue.dequeue_interval);
    printf("dequeue_timestamp = %-f\n", queue.dequeue_timestamp);
    printf("drop code         = %-d\n", queue.drop_code);
}
#endif