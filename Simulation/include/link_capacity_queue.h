#define LINK_CAPACITY_QUEUE_H

#ifdef LINK_CAPACITY_QUEUE_H

#define ALPHA 1
#define BETA 0

typedef struct
{
    int *alpha_ptr_index;
    int *beta_ptr_index;

    int max_buffer;

    int alpha_front;
    int alpha_rear;
    int beta_front;
    int beta_rear;

    TIME_TYPE dequeue_timestamp;
    TIME_TYPE dequeue_interval;
} link_capacity_queue;

void initLinkQueue(link_capacity_queue *pqueue, int max_buffer, const configuration config, double bandwidth)
{
    pqueue->alpha_ptr_index = (int *)malloc(sizeof(int) * max_buffer);
    memset(pqueue->alpha_ptr_index, -1, max_buffer);
    pqueue->beta_ptr_index = (int *)malloc(sizeof(int) * max_buffer);
    memset(pqueue->beta_ptr_index, -1, max_buffer);

    pqueue->max_buffer = max_buffer;

    pqueue->alpha_front = -1;
    pqueue->alpha_rear = -1;
    pqueue->beta_front = -1;
    pqueue->beta_rear = -1;

    pqueue->dequeue_timestamp = 0;
    pqueue->dequeue_interval = (TIME_TYPE)(config.packet_size * (double)ONE_SECOND_IN_NS / (bandwidth * config.unit));
}

int isLinkFull(link_capacity_queue *pqueue, int code)
{
    if (code == ALPHA)
        return pqueue->alpha_rear == pqueue->max_buffer - 1;
    else
        return (pqueue->alpha_rear - pqueue->alpha_front + 1) + (pqueue->beta_rear - pqueue->beta_front + 1) == pqueue->max_buffer - 1;
}

int isLinkEmpty(link_capacity_queue *pqueue, int code)
{
    if (code == ALPHA)
        return pqueue->alpha_front == -1;
    else
        return pqueue->beta_front == -1;
}

int enqueueLink(link_capacity_queue *pqueue, int tenant, int code)
{
    if (isLinkFull(pqueue, code))
    {
        // printf("Queue is full. front = %d, rear = %d.\n", pqueue->front, pqueue->rear);
        return PACKET_LABEL_OVER_CAPACITY_DROPPED;
    }
    else
    {
        if (code == ALPHA)
        {
            if (pqueue->alpha_front == -1)
            {
                pqueue->alpha_front = 0; // If it's the first packet
            }
            pqueue->alpha_ptr_index[pqueue->alpha_rear] = tenant;
            pqueue->alpha_rear++;
        }
        else
        {
            if (pqueue->beta_front == -1)
            {
                pqueue->beta_front = 0; // If it's the first packet
            }
            pqueue->beta_ptr_index[pqueue->beta_rear] = tenant;
            pqueue->beta_rear++;
        }
        return PACKET_LABEL_ACCEPT;
    }
}

void dequeueLink(link_capacity_queue *pqueue)
{
    if (isLinkEmpty(pqueue, ALPHA))
    {
        if (isLinkEmpty(pqueue, BETA))
            return; // Queue is empty, nothing to transmit
        else
        {
            pqueue->beta_front++;
            // printf("dequeue front = %d")
            if (pqueue->beta_front > pqueue->beta_rear)
            {
                pqueue->beta_front = -1; // Reset queue if it's empty
                pqueue->beta_rear = -1;
            }
        }
    }
    else
    {
        pqueue->alpha_front++;
        // printf("dequeue front = %d")
        if (pqueue->alpha_front > pqueue->alpha_rear)
        {
            pqueue->alpha_front = -1; // Reset queue if it's empty
            pqueue->alpha_rear = -1;
        }
        return;
    }
}

// int simulation_link(int index, link_capacity_queue *pqueue)
// {
//     if (index != PACKET_LABEL_ACCEPT)
//         return index;

//     int result = enqueue(pqueue);

//     if (!isEmpty(pqueue))
//     {
//         dequeue(pqueue);
//     }

//     return result;
// }

// void record_queue_packets(int* packets, const char* packets_data_path){
//     FILE* file = fopen(packets_data_path, "w");
//     if (!file) {
//         perror("Error opening file for writing");
//         return;
//     }
//     for (int index = 0;index<MAX_QUEUE_SIZE;index++) {
//         fprintf(file, "%d", *(packets+index));
//         if (index < MAX_QUEUE_SIZE-1)
//             fprintf(file, ",");
//     }
//     fclose(file);
// }

// void record_link_capacity_queue(link_capacity_queue queue, const char* folder_path){
//     char data_path[MAX_PATH_LENGTH];

//     sprintf(data_path, "%s/link_queue/front.txt", folder_path);
//     FILE* link_file = fopen(data_path, "w");
//     if (!link_file) {
//         printf("Unable to open file  \"%s\" for writing\n", data_path);
//         perror("Unable to open file for writing");
//         return;
//     }
//     fprintf(link_file, "%d\n", queue.front);
//     fclose(link_file);

//     sprintf(data_path, "%s/link_queue/rear.txt", folder_path);
//     link_file = fopen(data_path, "w");
//     if (!link_file) {
//         printf("Unable to open file  \"%s\" for writing\n", data_path);
//         perror("Unable to open file for writing");
//         return;
//     }
//     fprintf(link_file, "%d\n", queue.rear);
//     fclose(link_file);

//     sprintf(data_path, "%s/link_queue/packets.csv", folder_path);
//     record_queue_packets(queue.packets, data_path);
// }

// int read_rear_or_front(const char* data_path){
//     FILE* file = fopen(data_path, "r");
//     if (!file) {
//         perror("Error opening file");
//         exit(EXIT_FAILURE); // Terminate program if file can't be opened
//     }

//     int value;
//     if (fscanf(file, "%d", &value) != 1) { // Try to read an integer
//         fprintf(stderr, "Error reading integer from file\n");
//         fclose(file);
//         exit(EXIT_FAILURE); // Terminate program if no integer is found
//     }

//     fclose(file);
//     return value;
// }

// int* read_packets_from_csv(const char* filename) {
//     FILE* file = fopen(filename, "r");
//     if (!file) {
//         perror("Error opening file for reading");
//         return NULL;
//     }

//     char buffer[MAX_BUFFER_SIZE];
//     if (!fgets(buffer, sizeof(buffer), file)) {
//         perror("Error reading from file");
//         fclose(file);
//         return NULL;
//     }
//     fclose(file);

//     int* array = (int*)malloc(MAX_QUEUE_SIZE * sizeof(int));
//     if (!array) {
//         perror("Memory allocation error");
//         return NULL;
//     }

//     char* token = strtok(buffer, ",");
//     size_t index = 0;
//     while (token) {
//         array[index++] = atoi(token);
//         token = strtok(NULL, ",");
//     }
//     return array;
// }

// void read_link_capacity_queue(link_capacity_queue* pqueue, const char* folder_path){
//     char data_path[MAX_PATH_LENGTH];
//     sprintf(data_path, "%s/link_queue/front.txt", folder_path);
//     pqueue->front = read_rear_or_front(data_path);
//     sprintf(data_path, "%s/link_queue/rear.txt", folder_path);
//     pqueue->rear = read_rear_or_front(data_path);
//     sprintf(data_path, "%s/link_queue/packets.csv", folder_path);

//     int* temp_packets = read_packets_from_csv(data_path);
//     for(int index=0;index<MAX_QUEUE_SIZE;index++)
//         *(pqueue->packets+index) = *(temp_packets+index);

// }

// void print_link_queue(link_capacity_queue queue){
//     printf("front = %d\n", queue.front);
//     printf("rear = %d\n", queue.rear);

// }

// void record_dequeue_timestamp(long dequeue_timestamp, const char* folder_path){
//     char data_path[MAX_PATH_LENGTH];
//     sprintf(data_path, "%s/link_queue/dequeue_timestamp.txt", folder_path);
//     FILE* file = fopen(data_path, "w+");
//     if (!file) {
//         perror("Error opening file for reading");
//         exit(EXIT_FAILURE);
//     }

//     fprintf(file, "%ld\n", dequeue_timestamp);
//     fclose(file);
// }

// void read_dequeue_timestamp(long* dequeue_timestamp, const char* folder_path){
//     char data_path[MAX_PATH_LENGTH];
//     sprintf(data_path, "%s/link_queue/dequeue_timestamp.txt", folder_path);
//     FILE* file = fopen(data_path, "r");
//     if (!file) {
//         perror("Error opening file");
//         exit(EXIT_FAILURE); // Terminate program if file can't be opened
//     }

//     long value;
//     if (fscanf(file, "%ld", &value) != 1) { // Try to read an integer
//         fprintf(stderr, "Error reading integer from file\n");
//         fclose(file);
//         exit(EXIT_FAILURE); // Terminate program if no integer is found
//     }

//     fclose(file);
//     *dequeue_timestamp = value;
// }
#endif