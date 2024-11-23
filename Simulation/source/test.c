
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"

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
    // printf("%ld\n", grid_length);

    double ratio = (double)(config.mean * unit) / GBPS;
    printf("%f\n", ratio);

    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }

    int tenant_number = 10;
    for (int tenant = 0; tenant < tenant_number; tenant++)
        fprintf(file, "%d", tenant);
    fprintf(file, "\n");

    long *count = (long *)calloc(tenant_number, sizeof(long));
    grid_length = 100;

    int **label = (int **)malloc(sizeof(int *) * tenant_number);
    for (int tenant = 0; tenant < tenant_number; tenant++)
        *(label + tenant) = (int *)calloc(4, sizeof(int));

    for (long grid = 0; grid < grid_length; grid++)
    {
        int *packets = packet_generation_uniform(grid, ratio, tenant_number);
        // print_packets(packets, tenant_number);
        for (int tenant = 0; tenant < tenant_number; tenant++)
        {
            if (tenant == tenant_number - 1)
                fprintf(file, "%d\n", *(packets + tenant));
            else
                fprintf(file, "%d, ", *(packets + tenant));

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                *(count + tenant) += 1;
        }
        // printf("\n");
    }
    fclose(file);

    printf("\ncount =\n");
    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("%-10ld :%f\n", *(count + tenant), (double)(*(count + tenant) * 100) / grid_length);
    printf("\n");
    // int *packets = packet_generation_uniform(0, ratio, grid_length);
    // print_packets(packets, grid_length);

    //     // Initialize tenants
    //     tenant* tenants = initialize_tenants(config.tenant_number, config.simulation_time);

    //     // Decide traffic mode
    //     for(int index = 0; index < config.tenant_number; index++){
    //         tenant t = (tenant)*(tenants+index);
    //         switch(config.traffic_mode){
    //             case TRAFFIC_MODE_INTERVAL:
    //                 tenants[index].traffic_mode = TENANT_TRAFFIC_MODE_INTERVAL;
    //             break;
    //             case TRAFFIC_MODE_GAUSSIAN:
    //                 tenants[index].traffic_mode = TENANT_TRAFFIC_MODE_GAUSSIAN;
    //             break;
    //             case TRAFFIC_MODE_ALL_NAUGHTY:
    //                 if(index>config.naughty_tenant_number)
    //                     tenants[index].traffic_mode = TENANT_TRAFFIC_MODE_INTERVAL;
    //                 else tenants[index].traffic_mode = TRAFFIC_MODE_ALL_NAUGHTY;
    //             break;
    //             default:
    //                 tenants[index].traffic_mode = TENANT_TRAFFIC_MODE_INTERVAL;
    //             break;
    //         }
    //     }

    //     if(atoi(argv[1])==GENERATE_TRAFFIC)
    //          tenants_timestamp_translation_external(CONFIGURATION_PATH, config.tenant_number, STORED_PACKET_GENERATION_PATH, STORED_TIMESTAMP_PATH);

    //     // const char *filename = "../data/test/timestamp/tenant_99.csv";
    //     // size_t size = 0;

    //     // long long *timestamps = read_csv(filename, &size);
    //     // printf("Timestamps (%zu entries):\n", size);
    //     // for (size_t i = 0; i < size; i++) {
    //     //     printf("%ld\n", timestamps[i]);
    //     // }

    //     // Free allocated memory
    //     free(timestamps);

    //     configuration config;

    //     if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
    //         printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
    //         return EXIT_FAILURE;
    //     }

    //     int identifier = 0;
    //     tenant t = initialize_tenant(identifier, config.simulation_time);
    //     // for (long index=0;index<t.simulation_time;index++)
    //     //     if(index<=(t.simulation_time/2))
    //     //         *(t.generated_packets+index) = 159;
    //     //     else *(t.generated_packets+index) = 161;
    //     tenant_packet_generation(&t, config.mean, config.standard_deviation, config.naughty_mean, config.naughty_standard_deviation);
    //     translate_band_to_packets(&t, MBPS, config.packet_size);

    // #ifdef RECORD_PACKETS_GENERATION
    //     record_generated_packets(&t, STORED_PACKET_GENERATION_PATH);
    // #endif

    //     timestamp_translation(&t);
    // #ifdef RECORD_TIMESTAMP
    //     record_timestamps(&t, STORED_TIMESTAMP_PATH);
    // #endif

    return EXIT_SUCCESS;
}