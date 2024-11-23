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

#endif