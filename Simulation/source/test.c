
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

int main(int argc, char *argv[]){


//     if(atoi(argv[1]) == GENERATE_TRAFFIC){
//         system("rm -r ../data/"NAME);
//         system("mkdir ../data/"NAME);
//         system("mkdir ../data/"NAME"/packet_generation");
//         system("mkdir ../data/"NAME"/packet_generation/images");
//         system("mkdir ../data/"NAME"/timestamp");
//         system("mkdir ../data/"NAME"/timestamp/images");
//     }


//     // configuration.h
//     configuration config;

//     if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
//         printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
//         return EXIT_FAILURE;
//     }

//     // show_configuration(config);

//     config.packet_size = 512;
//     modify_ini_file(CONFIGURATION_PATH, &config);

// #ifdef REDUCTION
//     reduction_inif_file(CONFIGURATION_PATH);
// #endif

//     // capacity.py
//     char command[MAX_COMMAND_LENGTH];
//     sprintf(command, "python3 "PYTHON_CAPACITY_CALCULATION_PATH" %s %d", CONFIGURATION_PATH, 0);
//     system(command);
  
//     double capacity = obtain_capacity();
//     printf("capacity : %f\n", capacity);

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

    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
        printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

//     int identifier = 0;
//     tenant t = initialize_tenant(identifier, config.simulation_time);
//     for (long index=0;index<t.simulation_time;index++)
//         if(index<=(t.simulation_time/2))
//             *(t.generated_packets+index) = 159;
//         else *(t.generated_packets+index) = 161;
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