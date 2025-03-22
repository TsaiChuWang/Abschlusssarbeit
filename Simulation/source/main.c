
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/state_machine.h"
#include "../include/traffic_generation.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/main.ini"

#define NAME "main"

// gcc ./main.c inih/ini.c -o ../execution/main -lm
// ../execution/main [traffic_generation_code]

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


    return EXIT_SUCCESS;
}