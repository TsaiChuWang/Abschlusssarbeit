
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"

#define CONFIGURATION_PATH "../configuration/simple_V1.ini"

#define NAME "test"
#define STORED_PACKET_GENERATION_PATH "../data/test/packet_generation"
#define STORED_TIMESTAMP_PATH "../data/test/timestamp"

// gcc ./test.c inih/ini.c -o ../execution/test -lm
// ../execution/test [traffic_generation_code]

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{

    char command[MAX_COMMAND_LENGTH];
    // if(atoi(argv[1]) == GENERATE_TRAFFIC){
    //     system("rm -r ../data/"NAME);
    //     system("mkdir ../data/"NAME);
    //     system("mkdir ../data/"NAME"/packet_generation");
    //     system("mkdir ../data/"NAME"/packet_generation/images");
    //     system("mkdir ../data/"NAME"/timestamp");
    //     system("mkdir ../data/"NAME"/timestamp/images");
    // }

    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    // show_configuration(config);

    // config.packet_size = 512;
    // modify_ini_file(CONFIGURATION_PATH, &config);

#ifdef REDUCTION
    reduction_inif_file(CONFIGURATION_PATH);
#endif

    long unit;
    switch (config.unit)
    {
    case UNIT_MBPS:
        unit = MBPS;
        break;
    case UNIT_KBPS:
        unit = KBPS;
        break;
    default:
        unit = MBPS;
    }

    // Clean
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);
    // sprintf(command, "mkdir %s/packets", config.data_path);
    // system(command);
    // sprintf(command, "mkdir %s/packets/images", config.data_path);
    // system(command);

    // capacity.py
    sprintf(command, "python3 " PYTHON_CAPACITY_CALCULATION_PATH " %s %d", CONFIGURATION_PATH, 0);
    system(command);

    double capacity = obtain_capacity();
    printf("capacity : %f bps\n", capacity * unit);

    long step_size = (long)((long)config.packet_size / (GBPS / ONE_SECOND_IN_NS));
    long grid_length = obtain_grid_length(config.simulation_time, step_size);
    printf("%ld\n", grid_length);

    double ratio = (double)(config.mean * unit) / GBPS;
    // printf("%f\n", ratio);

    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    // int tenant_number = config.tenant_number;
    int tenant_number = 10;
    for (int tenant = 0; tenant < tenant_number; tenant++)
        if (tenant == tenant_number - 1)
            fprintf(file, "%d\n", tenant);
        else
            fprintf(file, "%d, ", tenant);

    long *count = (long *)calloc(tenant_number, sizeof(long));
    grid_length = 1171875;

    int **label = (int **)malloc(sizeof(int *) * tenant_number);
    for (int tenant = 0; tenant < tenant_number; tenant++)
        *(label + tenant) = (int *)calloc(4, sizeof(int));

    GCRA *gcras_1 = initializeGCRAs(tenant_number, config.tau_1, config.packet_size);
    GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
    link_capacity_queue link;
    initQueue(&link);
    long linkTransmissionInterval = (long)(config.packet_size*(double)ONE_SECOND_IN_NS/(capacity*unit));   //config.packet_size
    printf("linkTransmissionInterval = %ld\n", linkTransmissionInterval);
    long dequeue_timestamp = 0;

    for (long grid = 0; grid < grid_length; grid++)
    {
        int *packets = packet_generation_uniform(grid, ratio, tenant_number);
        // print_packets(packets, tenant_number);
        long timestamp = grid * step_size;

            // printf("%ld %ld\n", dequeue_timestamp, timestamp);
        while(dequeue_timestamp <= timestamp){
            dequeue(&link);
            dequeue_timestamp += linkTransmissionInterval;
            // printf("%ld %ld\n", dequeue_timestamp, timestamp);
        }

        for (int tenant = 0; tenant < tenant_number; tenant++)
        {
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                    *(count + tenant) += 1;

                GCRA *gcra_1 = (GCRA *)(gcras_1 + tenant);
                long x = (long)gcra_1->x - (timestamp - gcra_1->last_time) * (((double)(config.mean + config.standard_deviation) * unit) / ONE_SECOND_IN_NS);

                // printf("%ld %ld %f\n", gcra_1->x, grid * step_size, (timestamp - gcra_1->last_time) * (((double)(config.mean + config.standard_deviation) * unit) / ONE_SECOND_IN_NS));

                if (x > gcra_1->tau)
                    *(packets + tenant) = PACKET_LABEL_OVER_UPPERBOUND_DROPPED;
                else
                {
                    gcra_1->x = MAX((long)0, x) + gcra_1->l;
                    gcra_1->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }

                if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                {
                    GCRA *gcra_2 = (GCRA *)(gcras_2 + tenant);
                    
                    long x = (long)gcra_2->x - (timestamp - gcra_2->last_time) * (((double)(config.mean) * unit) / ONE_SECOND_IN_NS);

                    // printf("%ld %ld %f\n", gcra_2->x, grid * step_size, (timestamp - gcra_2->last_time) * (((double)(config.mean) * unit) / ONE_SECOND_IN_NS));

                    if (x > gcra_2->tau)
                        *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
                    else
                    {
                        gcra_2->x = MAX((long)0, x) + gcra_2->l;
                        gcra_2->last_time = timestamp;
                        *(packets + tenant) = PACKET_LABEL_ACCEPT;
                    }
                }
                else
                    goto RECORD;

                if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                    *(packets + tenant) = enqueue(&link);
                }
                else
                    goto RECORD;
            }
            else
                goto RECORD;

            // Record
        RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET)
                label[tenant][*(packets + tenant)] += 1;

            if (tenant == tenant_number - 1)
                fprintf(file, "%d\n", *(packets + tenant));
            else
                fprintf(file, "%d, ", *(packets + tenant));
        }
        // printf("\n");

        // printf("%d\n", link.rear);
    }
    fclose(file);

    print_packets_count(count, tenant_number, grid_length);
    record_packets_count(count, tenant_number, grid_length, config.data_path);

    
    printf("\ncount = %ld\n", config.tau_2);
    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("%-10ld %-10ld %-10ld %-10ld\n", label[tenant][0], label[tenant][1], label[tenant][2], label[tenant][3]);
    printf("\n");


    return EXIT_SUCCESS;
}