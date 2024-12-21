#include <math.h>

#define PYTHON_PACKET_GENERATION_PATH "../python/packet_generation.py"
#define EXTERNAL_SOURCE_PATH "traffic_generation_tenant"

#define TRAFFIC_GENERATION_H

#ifdef TRAFFIC_GENERATION_H

typedef struct
{
    long step_size;
    long grids_number;

    double generate_probability;
    double generate_probability_naughty;
} traffic_generator;

traffic_generator initializeTrafficGenerator(const configuration config)
{
    traffic_generator generate;

    double temp = 0;
    temp = (double)ONE_SECOND_IN_NS / config.input_rate;
    generate.step_size = (long)(config.packet_size * temp);
    generate.grids_number = (long)((double)ONE_SECOND_IN_NS / generate.step_size);

    long unit = obtainUnit(config);
    generate.generate_probability = (double)config.mean * unit / config.input_rate;
    generate.generate_probability_naughty = (double)config.naughty_mean * unit / config.input_rate;

    return generate;
}

long obtain_grid_length(int simulation_time, int step_size)
{
    return 1953125;
}

long obtain_grids_number(int packet_size)
{
    return (long)ONE_SECOND_IN_NS / packet_size;
}

void showTrafficGenerator(const traffic_generator generate)
{
    printf("| step_size                    : %-ld\n", generate.step_size);
    printf("| grids_number                 : %-ld\n", generate.grids_number);
    printf("| generate_probability         : %-f\n", generate.generate_probability);
    printf("| generate_probability_naughty : %-f\n", generate.generate_probability_naughty);
}

int uniform_distribution(int random, double probability)
{
    srand((unsigned int)time(NULL) + random);
    double factor = (rand() / (double)RAND_MAX);
    if (factor <= probability)
        return PACKET_LABEL_ACCEPT;
    else
        return PACKET_LABEL_NO_PACKET;
}

int *packet_generation_uniform(int random, double probability, int tenant_number)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number);
    for (long index = 0; index < tenant_number; index++)
    {
        *(packets + index) = uniform_distribution(random + index, probability);
        // printf("[%-10ld] : %-d\n", *(packets + index));
    }
    return packets;
}
// int* packets = packet_generation_uniform(0, generator.generate_probability, config.tenant_number);
// print_packets(packets, config.tenant_number);

int *packet_generation_naughty(int random, double ratio_normal, double ratio_naughty, int tenant_number, int naughty_tenant_number)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number);
    for (int index = 0; index < tenant_number; index++)
    {
        if (index < naughty_tenant_number && GENERATE_BINARY)
            *(packets + index) = uniform_distribution(random + index, ratio_naughty);
        else
            *(packets + index) = uniform_distribution(random + index, ratio_normal);

        // printf("[%-10ld] : %-d\n", *(packets + index));
    }
    return packets;
}

void print_packets(int *packets, long tenant_number)
{
    long sum = 0;
    printf("sum = %ld, length = %ld %f\n", sum, tenant_number, (double)sum / tenant_number);
    for (long index = 0; index < tenant_number; index++)
    {
        if (*(packets + index) == PACKET_LABEL_ACCEPT)
            sum += 1;

        printf("[%-10ld] : %-d\n", index, *(packets + index));
    }

    printf("sum = %ld, length = %ld %f\n", sum, tenant_number, (double)sum / tenant_number);
}

// void record_paclets(const char)

#endif