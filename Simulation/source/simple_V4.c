
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/packets_count.h"

// #define CONFIGURATION_PATH "../configuration/simple_V3.ini"
#define CONFIGURATION_PATH "../configuration/simple_V3.ini"

#define NAME "test"
#define STORED_PACKET_GENERATION_PATH "../data/test/packet_generation"
#define STORED_TIMESTAMP_PATH "../data/test/timestamp"

// gcc ./simple_V4.c inih/ini.c -o ../execution/simple_V4 -lm
// ../execution/simple_V4 [traffic_generation_code]

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
    // long unit = obtainUnit(config);
    // // show_configuration(config);

    system("gcc ./simple_V3.c inih/ini.c -o ../execution/simple_V3 -lm");

    system("rm ../data/tau_variation.csv");
    FILE *file = fopen("../data/tau_variation.csv", "a");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "tau, regular_loss, naughty_loss\n");
    fclose(file);

    for (long tau = 1000448; tau >= 512; tau += 1024)
    {
        config.tau = tau;
        modify_ini_file(CONFIGURATION_PATH, &config);

        file = fopen("../data/tau_variation.csv", "a");
        if (!file)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%ld, ", tau);
        fclose(file);
        system("../execution/simple_V3");
    }

    return 0;
}