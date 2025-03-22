
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP

#define PRINT_EXECUTION_TIME 
#define PRINT_CAPACITY
// #define PRINT_PACKET_COUNTS
#define PRINT_PACKET_LABEL
#define PRINT_REGULAR_AND_NAUGHTY

#define RECORD_REGULAR_AND_NAUGHTY_TAU
#define RECORD_REGULAR_AND_NAUGHTY_ALL
#define RECORD_AVERAGE_LOSS

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/packets_count.h"


// #include "../include/link_capacity_queue.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"


#define CONFIGURATION_PATH "../configuration/main.ini"

#define NAME "main"

// gcc ./main.c inih/ini.c -o ../execution/main -lm
// ../execution/main [traffic_generation_code]

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];
#ifdef PRINT_EXECUTION_TIME 
    clock_t execute_clock = clock();
#endif

    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }
    // show_configuration(config);

    // Clean
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);

    // Calculate the capacity
    double capacity = obtain_capacity();
#ifdef PRINT_CAPACITY
    printf("capacity : %f bps\n", capacity * config.unit);
#endif

    // Set random seed
    srand48(time(NULL));

    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);

    // Float variable
    int tenant_number = config.tenant_number;
    long grids_number = generator.grids_number;
    TIME_TYPE timestamp = (TIME_TYPE)0;

    packets_count count;
    init_packets_count(&count, tenant_number, obtain_grids_number(config.packet_size));
    

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);


    

    // for(int i=0;i<10;i++){
    //     int* packets = packet_generation_configuration(generator, config);
    //     print_packets(packets, config.tenant_number);
    // }printf("\n");
    

#ifdef PRINT_PACKET_COUNTS
    show_packets_count(count);
#endif

#ifdef PRINT_PACKET_LABEL
    show_packets_label(label);        
#endif

#ifdef PRINT_REGULAR_AND_NAUGHTY
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
        print_regular_and_naughty(label, config.naughty_tenant_number);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_TAU
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
        record_regular_and_naughty_tau(label, config);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_ALL
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
        record_regular_and_naughty_all(label, config);
    }
#endif

#ifdef RECORD_AVERAGE_LOSS
    record_average_loss(label, config);
#endif

#ifdef PRINT_EXECUTION_TIME 
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f s\n", time_taken);
#endif

    return EXIT_SUCCESS;
}