
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

// #define ALL_UNIFORM_DISTRIBUTION 1

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
#include "../include/packets_count.h"

// #define CONFIGURATION_PATH "../configuration/simple_V2.ini"

#define NAME "test"
#define STORED_PACKET_GENERATION_PATH "../data/test/packet_generation"
#define STORED_TIMESTAMP_PATH "../data/test/timestamp"

// gcc ./skript.c inih/ini.c -o ../execution/skript -lm
// ../execution/skript [traffic_generation_code]

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];

    // configuration.h
    configuration config;

#define CONFIGURATION_PATH "../configuration/main.ini"
    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    char file_path[MAX_PATH_LENGTH];
    sprintf(file_path, "%s/record_average_loss.csv", config.data_path);

    FILE *file = fopen(file_path, "w+");
    if (!file)
    {
        perror("Error opening file"); /**< Handle file open errors. */
        exit(EXIT_FAILURE);
    }

    fprintf(file, RECORD_AVERAGE_LOSS_HEADER);

    fclose(file);

    long step = 256;
    for (long tau = 0; tau <= 153600000; tau += step)
    {
        config.tau = tau;
        modify_ini_file(CONFIGURATION_PATH, &config);

        system("../execution/main");

        if (tau >= step * 10)
            step *= 2;
    }

    return 0;
}