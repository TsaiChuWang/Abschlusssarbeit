#include <math.h>

#define PYTHON_PACKET_GENERATION_PATH "../python/packet_generation.py"
#define EXTERNAL_SOURCE_PATH "traffic_generation_tenant"

#define TRAFFIC_GENERATION_H

#ifdef TRAFFIC_GENERATION_H

typedef struct{
    int state;

    double r;
    double p;
}state_machine;

typedef struct
{
    TIME_TYPE step_size;
    long grids_number;

    double generate_probability;
    double generate_probability_naughty;

    state_machine* state;
} traffic_generator;

traffic_generator initializeTrafficGenerator(const configuration config)
{
    traffic_generator generate;

    double temp = 0;
    temp = (double)ONE_SECOND_IN_NS / config.input_rate;
    generate.step_size = (TIME_TYPE)(config.packet_size * temp);
    generate.grids_number = (long)((double)ONE_SECOND_IN_NS / generate.step_size);

    long unit = config.unit;
    generate.generate_probability = (double)config.mean * unit / config.input_rate;
    generate.generate_probability_naughty = (double)config.naughty_mean * unit / config.input_rate;
    
    if(config.traffic_mode == TRAFFIC_MODE_BRUSTY_ALL || config.traffic_mode == TRAFFIC_MODE_BRUSTY_REGULAR || config.traffic_mode == TRAFFIC_MODE_BRUSTY_NAUGHTY){
        generate.state = (state_machine*)malloc(sizeof(state_machine)*config.tenant_number);
        for(int i=0;i<config.tenant_number;i++){
            *(generate.state + i)->state = PACKET_LABEL_NO_PACKET;
            *(generate.state + i)->p = 0;
            *(generate.state + i)->r = 0;
        }
    }else generate.state = NULL;

    return generate;
}

long obtain_grids_number(int packet_size)
{
    return (long)ONE_SECOND_IN_NS / packet_size;
}

void showTrafficGenerator(const traffic_generator generate)
{
    printf("| step_size                    : %-lF\n", generate.step_size);
    printf("| grids_number                 : %-ld\n", generate.grids_number);
    printf("| generate_probability         : %-f\n", generate.generate_probability);
    printf("| generate_probability_naughty : %-f\n", generate.generate_probability_naughty);
}

typedef struct{
    int state;
    double p;
    double r;
}state;

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
        // if (index < naughty_tenant_number && GENERATE_BINARY)
        if (index >= naughty_tenant_number)
            *(packets + index) = uniform_distribution(random + index, ratio_naughty);
        else
            *(packets + index) = uniform_distribution(random + index, ratio_normal);

        // printf("[%-10ld] : %-d\n", *(packets + index));
    }
    return packets;
}

int *packet_generation_full(int tenant_number)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number);
    memset(packets, PACKET_LABEL_ACCEPT, sizeof(packets));
    return packets;
}

int *packet_generation_density(int tenant_number, int grid, int window)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number);
    for (int index = 0; index < tenant_number; index++)
    {
        double step = 327681.0 / window;
        double r = fmod(grid, step); // grid
        // printf("r = %d, step = %d\n", r, (int)(step*0.75));
        if (index % 2 == 0)
        {
            if (r < step * 0.75)
                *(packets + index) = PACKET_LABEL_ACCEPT;
            else
                *(packets + index) = PACKET_LABEL_NO_PACKET;
        }
        else
        {
            if (r < step * 0.25)
                *(packets + index) = PACKET_LABEL_NO_PACKET;
            else
                *(packets + index) = PACKET_LABEL_ACCEPT;
        }

        // if(r<step-(int)(step*0.75))
        //     *(packets + index) = PACKET_LABEL_NO_PACKET;
        // else
        //     *(packets + index) = PACKET_LABEL_ACCEPT;
    }
    return packets;
}

void print_packets(int *packets, long tenant_number)
{
    long sum = 0;
    // printf("sum = %ld, length = %ld %f\n", sum, tenant_number, (double)sum / tenant_number);
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