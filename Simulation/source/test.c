
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

// #define CONFIGURATION_PATH "../configuration/simple_V1.ini"
#define CONFIGURATION_PATH "../configuration/simple_V1_naughty.ini"

#define NAME "test"
#define STORED_PACKET_GENERATION_PATH "../data/test/packet_generation"
#define STORED_TIMESTAMP_PATH "../data/test/timestamp"

// gcc ./test.c inih/ini.c -o ../execution/test -lm
// ../execution/test [traffic_generation_code]

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

    
    // print_packets(packets, config.tenant_number);

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

    // system("gcc ./single_window.c inih/ini.c -o ../execution/single_window -lm");
    // for(long window = 0;window<window_length;window++){
    //     sprintf(command, "../execution/single_window %ld %f", window, capacity);
    //     system(command);
    
    //     sprintf(command, "python3 ../python/statistics.py %s/label.csv", config.data_path);
    //     system(command);
    // }
    system("gcc ./single_window_naughty.c inih/ini.c -o ../execution/single_window_naughty -lm");
    for(long window = 0;window<window_length;window++){
        sprintf(command, "../execution/single_window_naughty %ld %f", window, capacity);
        system(command);
    
        sprintf(command, "python3 ../python/statistics.py %s/label.csv", config.data_path);
        system(command);
    }

    // char filename[MAX_PATH_LENGTH];
    // FILE *file;


    // system("gcc ./single_grid.c inih/ini.c -o ../execution/single_grid -lm");
    // for(long window = 0;window<window_length;window++){
    //     sprintf(filename, "%s/packets.csv", config.data_path);
    //     file = fopen(filename, "w");
    //     if (file == NULL){
    //         printf("Failed to open file %s for writing.\n", filename);
    //         exit(EXIT_FAILURE);
    //     }
    //     fclose(file);

    //     for (long grid = 0; grid < grid_length; grid++){
    //         sprintf(command, "../execution/single_grid %d %d %ld %f", grid, window, capacity);
    //         system(command);
    //     }
    // }
    // for(long window = 0;window<window_length;window++){
    //     execute_clock = clock() - execute_clock;
    //     double time_taken = ((double)execute_clock)/CLOCKS_PER_SEC;
    //     // printf("(%6.4f), Window = %ld, Time start at %ld : Execute time : %f\n", (double)window*100.0/config.simulation_time,window, window*step_size*grid_length,  time_taken);
  
    //     sprintf(filename, "%s/packets.csv", config.data_path);
    //     file = fopen(filename, "w");
    //     if (file == NULL){
    //         printf("Failed to open file %s for writing.\n", filename);
    //         exit(EXIT_FAILURE);
    //     }

    //     for (int tenant = 0; tenant < tenant_number; tenant++){
    //         for (long grid = 0; grid < grid_length; grid++){

    //             sprintf(command, "../execution/single_grid %d %d %ld %f", grid, window, capacity);
    //             system(command);
    //             // printf("%s\n", command);
    //             // print_packets_count(count);
    //         }
    //     }
        
    //     fclose(file);
    // }

//   int grid = atoi(argv[1]);
//   int window = atoi(argv[2]);
//   long dequeue_timestamp = strtol(argv[3], NULL, 10);
//   double capacity = atof(argv[4]);

//     long step_size = (long)((long)config.packet_size / (GBPS / ONE_SECOND_IN_NS));
//     printf("step_size = %ld\n", step_size);
//     long window_length = obtain_grid_length(config.simulation_time, step_size);
//     long grid_length = ONE_SECOND_IN_NS/config.packet_size;
//     window_length = window_length/grid_length;
//     printf("grid_length = %ld\n", grid_length);
//     printf("window_length = %ld\n", window_length);

//     printf("Max Time = %ld\n", window_length*step_size*grid_length);

//     double ratio = (double)(config.mean * unit) / GBPS;
//     // printf("%f\n", ratio);

//     int tenant_number = config.tenant_number;
//     // int tenant_number = 10;

//     long *count = (long *)calloc(tenant_number, sizeof(long));
//     int **label = (int **)malloc(sizeof(int *) * tenant_number);
//     for (int tenant = 0; tenant < tenant_number; tenant++)
//         *(label + tenant) = (int *)calloc(4, sizeof(int));

//     GCRA *gcras_1 = initializeGCRAs(tenant_number, config.tau_1, config.packet_size);
//     GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
//     link_capacity_queue link;
//     initQueue(&link);
//     long linkTransmissionInterval = (long)(config.packet_size*(double)ONE_SECOND_IN_NS/(capacity*unit));   //config.packet_size
//     // printf("linkTransmissionInterval = %ld\n", linkTransmissionInterval);
//     long dequeue_timestamp = 0;

//     // window_length = 2;
//     for(long window = 0;window<window_length;window++){
//         execute_clock = clock() - execute_clock;
//         double time_taken = ((double)execute_clock)/CLOCKS_PER_SEC;
//         printf("Window = %ld, Time start at %ld : Execute time : %f\n", window, window*step_size*grid_length,  time_taken);

//         // Open File to Record packets
//         char filename[MAX_PATH_LENGTH];
//         sprintf(filename, "%s/packets.csv", config.data_path);

//         FILE *file = fopen(filename, "w");
//         if (file == NULL){
//             printf("Failed to open file %s for writing.\n", filename);
//             exit(EXIT_FAILURE);
//         }  
//         for (int tenant = 0; tenant < tenant_number; tenant++)
//             if (tenant == tenant_number - 1)
//                 fprintf(file, "%d\n", tenant);
//             else fprintf(file, "%d, ", tenant);

//         for (long grid = 0; grid < grid_length; grid++){
//             int *packets = packet_generation_uniform(grid, ratio, tenant_number);
//             // print_packets(packets, tenant_number);
//             long timestamp = window*step_size*grid_length + grid * step_size;
//             // printf("timestamp = %-ld\n", timestamp);

//             while(dequeue_timestamp <= timestamp){
//                 dequeue(&link);
//                 dequeue_timestamp += linkTransmissionInterval;
//                 // printf("%ld %ld\n", dequeue_timestamp, timestamp);
//             }

//             for (int tenant = 0; tenant < tenant_number; tenant++)
//             {
//                 if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
//                     *(count + tenant) += 1;
//                 else goto RECORD;

//                 if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
//                     long rate_1 = (long)(timestamp - (gcras_1 + tenant)->last_time) * (((double)(config.mean + config.standard_deviation) * unit) / ONE_SECOND_IN_NS);
//                     // printf("rate = %ld\n", rate_1);
//                     long x = (long)(gcras_1 + tenant)->x - rate_1;
//                     // printf("rate = %ld\n", x);

//                     if (x > (gcras_1 + tenant)->tau)
//                         *(packets + tenant) = PACKET_LABEL_OVER_UPPERBOUND_DROPPED;
//                     else{
//                         (gcras_1 + tenant)->x = MAX((long)0, x) + (gcras_1 + tenant)->l;
//                         (gcras_1 + tenant)->last_time = timestamp;
//                         *(packets + tenant) = PACKET_LABEL_ACCEPT;
//                     }
//                 }else goto RECORD;

//                 if(*(packets + tenant) == PACKET_LABEL_ACCEPT){
//                     long rate_2 = (long)(timestamp - (gcras_2 + tenant)->last_time) * (((double)(config.mean) * unit) / ONE_SECOND_IN_NS);
//                     // printf("rate = %ld\n", rate_2);
//                     long x = (long)(gcras_2 + tenant)->x - rate_2;
//                     // printf("rate = %ld\n", x);

//                     if (x > (gcras_2 + tenant)->tau)
//                         *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
//                     else{
//                         (gcras_2 + tenant)->x = MAX((long)0, x) + (gcras_2 + tenant)->l;
//                         (gcras_2 + tenant)->last_time = timestamp;
//                         *(packets + tenant) = PACKET_LABEL_ACCEPT;
//                     }
//                 }else goto RECORD;

//                 if(*(packets + tenant) == PACKET_LABEL_ACCEPT)
//                     *(packets + tenant) = enqueue(&link);
//                 else goto RECORD;
//                 // Record
// RECORD:
//                 if (*(packets + tenant) != PACKET_LABEL_NO_PACKET)
//                     label[tenant][*(packets + tenant)] += 1;

//                 if (tenant == tenant_number - 1)
//                     fprintf(file, "%d\n", *(packets + tenant));
//                 else
//                     fprintf(file, "%d, ", *(packets + tenant));
//             }
//         }
//         fclose(file);

//         print_packets_count(count, tenant_number, grid_length*(window+1));
//         record_packets_count(count, tenant_number, grid_length*(window+1), config.data_path);

//         print_packets_label(label, count, tenant_number);
//         record_packets_label(label, count, tenant_number, config.data_path);
//     }

    return EXIT_SUCCESS;
}