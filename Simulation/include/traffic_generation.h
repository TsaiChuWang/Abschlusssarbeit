#include <math.h>

#define PYTHON_PACKET_GENERATION_PATH "../python/packet_generation.py"
#define EXTERNAL_SOURCE_PATH "traffic_generation_tenant"

#define TRAFFIC_GENERATION_H

#ifdef TRAFFIC_GENERATION_H

long obtain_grid_length(long simulation_time, long step_size)
{
    return (long)(simulation_time * ONE_SECOND_IN_NS / step_size);
}

int uniform_distribution(int random, double ratio)
{
    srand((unsigned int)time(NULL) + random);
    double factor = (rand() / (double)RAND_MAX);
    if (factor <= ratio)
        return PACKET_LABEL_ACCEPT;
    else
        return PACKET_LABEL_NO_PACKET;
}

int *packet_generation_uniform(int random, double ratio, long grid_length)
{
    int *packets = (int *)malloc(sizeof(int) * grid_length);
    for (long index = 0; index < grid_length; index++)
    {
        *(packets + index) = uniform_distribution(random + index, ratio);
        // printf("[%-10ld] : %-d\n", *(packets + index));
    }
    return packets;
}

void print_packets(int *packets, long grid_length)
{
    long sum = 0;
    printf("sum = %ld, length = %ld %f\n", sum, grid_length, (double)sum / grid_length);
    for (long index = 0; index < grid_length; index++)
    {
        if (*(packets + index) == PACKET_LABEL_ACCEPT)
            sum += 1;

        printf("[%-10ld] : %-d\n", index, *(packets + index));
    }

    printf("sum = %ld, length = %ld %f\n", sum, grid_length, (double)sum / grid_length);
}

// void record_paclets(const char)

void print_packets_count(long* count, int tenant_number, long grid_length){
    printf("\ncount =\n");
    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("Number of packets : %-10ld Percentage : %f %\n", *(count + tenant), (double)(*(count + tenant) * 100) / grid_length);
    printf("\n");
}

void record_packets_count(long* count, int tenant_number, long grid_length, const char* folder_path){
    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/count.csv", folder_path);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "tenant, packets, percentage\n");
    for (int tenant = 0; tenant < tenant_number; tenant++)
        fprintf(file, "%d, %ld, %f\n", tenant, *(count + tenant), (double)(*(count + tenant) * 100) / grid_length);
    fclose(file);
}

void print_packets_label(int** label, long* count, int tenant_number){
    printf("\nlabel =\n");
    printf("ACCEPT    , DROP_OVER , DROP_GCRA , CAPACITY   : LOSS\n");
    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("%-10d, %-10d, %-10d, %-10d : %-f % \n", label[tenant][0], label[tenant][1], label[tenant][2], label[tenant][3], (double)(label[tenant][2]+label[tenant][3])*100.0/ *(count+tenant));
    printf("\n");
}

void record_packets_label(int** label, long* count, int tenant_number, const char* folder_path){
    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/label.csv", folder_path);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    fprintf(file, "ACCEPT, DROP_OVER, DROP_GCRA, CAPACITY, LOSS\n");
    for (int tenant = 0; tenant < tenant_number; tenant++)
        fprintf(file, "%-10d, %-10d, %-10d, %-10d, %-f % \n", label[tenant][0], label[tenant][1], label[tenant][2], label[tenant][3], (double)(label[tenant][2]+label[tenant][3])*100.0/ *(count+tenant));
    fclose(file);
}
#endif