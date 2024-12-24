
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
#include "../include/queue.h"
#include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/simple_V3.ini"

#define NAME "simple_V3"

// gcc ./simple_V3.c inih/ini.c -o ../execution/simple_V3 -lm
// ../execution/simple_V3

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
    long unit = obtainUnit(config);
    // show_configuration(config);
    int tenant_number = config.tenant_number;

    // Clean
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s/link_queue_%d", config.data_path, PACKET_LABEL_OVER_CAPACITY_DROPPED);
    system(command);
    sprintf(command, "mkdir %s/link_queue_%d", config.data_path, PACKET_LABEL_OVER_UPPERBOUND_DROPPED);
    system(command);

    sprintf(command, "python3 " PYTHON_CAPACITY_CALCULATION_PATH " %s %d", CONFIGURATION_PATH, 0);
    system(command);

    double capacity = obtain_capacity();
    printf("capacity : %f bps\n", capacity * unit);

    queue link;
    int max_link_queue_buffer = 100;
    initQueue(&link, max_link_queue_buffer, config, capacity, PACKET_LABEL_OVER_CAPACITY_DROPPED);
    record_queue(link, config.data_path);
    // print_link_queue(link);

    packets_count count;
    init_Packets_Count(&count, tenant_number, obtain_grids_number(config.packet_size));
    record_packets_count(count, config.data_path);

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);
    record_packets_label(label, config.data_path);
    // print_packets_label(label);

    queue *shaping = initQueues(config.tau_1, config, (double)(config.mean + config.standard_deviation), PACKET_LABEL_OVER_UPPERBOUND_DROPPED, tenant_number);
    record_queues(shaping, config.data_path, tenant_number);

    GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
    record_gcras(gcras_2, tenant_number, config.data_path, 2);

    system("gcc ./single_one_second.c inih/ini.c -o ../execution/single_one_second -lm");
    for (long window = 0; window < config.simulation_time; window++)
    {
        sprintf(command, "../execution/single_one_second %ld %f %s", window, capacity, CONFIGURATION_PATH);
        system(command);

        sprintf(command, "python3 ../python/statistics.py %s/label.csv", config.data_path);
        system(command);
    }

    sprintf(command, "python3 ../python/chart.py %s", CONFIGURATION_PATH);
    system(command);

    sprintf(command, "python3 ../python/bandwidth.py %s", CONFIGURATION_PATH);
    system(command);

    return EXIT_SUCCESS;
}