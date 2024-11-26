
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"
#include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/simple_V1_naughty.ini"

#define NAME "simple_V1"

// gcc ./simple_V1.c inih/ini.c -o ../execution/simple_V1 -lm
// ../execution/simple_V1

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];


    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }
    // show_configuration(config);

    long unit;
    switch (config.unit){
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
    sprintf(command, "mkdir %s/link_queue", config.data_path);
    system(command);

    sprintf(command, "python3 " PYTHON_CAPACITY_CALCULATION_PATH " %s %d", CONFIGURATION_PATH, 0);
    system(command);

    double capacity = obtain_capacity();
    printf("capacity : %f bps\n", capacity * unit);

    long step_size = (long)((long)config.packet_size / (GBPS / ONE_SECOND_IN_NS));
    long window_length = obtain_grid_length(config.simulation_time, step_size);
    long grid_length = ONE_SECOND_IN_NS/config.packet_size;
    window_length = window_length/grid_length;
    // long grid_length = 20000;
    
    long dequeue_timestamp = 0;
    record_dequeue_timestamp(dequeue_timestamp, config.data_path);
    int tenant_number = config.tenant_number;
    // int tenant_number = 10;

    link_capacity_queue link;
    initQueue(&link);
    record_link_capacity_queue(link, config.data_path);

    packets_count count;
    init_Packets_Count(&count, tenant_number, grid_length);
    record_packets_count(count, config.data_path);

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);
    record_packets_label(label, config.data_path);
    // print_packets_label(label);

    GCRA *gcras_1 = initializeGCRAs(tenant_number, config.tau_1, config.packet_size);
    record_gcras(gcras_1, tenant_number, config.data_path, 1);

    GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
    record_gcras(gcras_2, tenant_number, config.data_path, 2);

    system("gcc ./single_window.c inih/ini.c -o ../execution/single_window -lm");
    for(long window = 0;window<window_length;window++){
        sprintf(command, "../execution/single_window %ld %f", window, capacity);
        system(command);
    
        sprintf(command, "python3 ../python/statistics.py %s/label.csv", config.data_path);
        system(command);
    }

    sprintf(command, "python3 ../python/chart.py %s", CONFIGURATION_PATH);
    system(command);
    return EXIT_SUCCESS;
}