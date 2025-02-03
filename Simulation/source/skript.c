
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

// #define CONFIGURATION_PATH "../configuration/simple_V2.ini"
#define CONFIGURATION_PATH "../configuration/simple_V2.ini"

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
    // long unit = obtainUnit(config);
    // // show_configuration(config);

    // // Clean
    // sprintf(command, "rm -r %s", config.data_path);
    // system(command);
    // sprintf(command, "mkdir %s", config.data_path);
    // system(command);
    // sprintf(command, "mkdir %s/link_queue", config.data_path);
    // system(command);

    // sprintf(command, "python3 " PYTHON_CAPACITY_CALCULATION_PATH " %s %d", CONFIGURATION_PATH, 0);
    // system(command);

    // double capacity = obtain_capacity();
    // printf("capacity : %f bps\n", capacity * unit);

    // traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);
    // long grids_number = generator.grids_number;

    system("gcc ./simple_V2.c inih/ini.c -o ../execution/simple_V2 -lm");

    system("rm ../data/naughty.csv");
    FILE *file = fopen("../data/naughty.csv", "a");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "tau, naughty_mean, regular, regular_loss, naughty_loss\n");
    fclose(file);


    for(int n =130;n<=150;n+=10){
        config.naughty_mean = n;
        modify_ini_file(CONFIGURATION_PATH, &config);

        for (long i = 0; i <= 10000; i += 512)
        {
            config.tau = i;
            modify_ini_file(CONFIGURATION_PATH, &config);
            file = fopen("../data/naughty.csv", "a");
            if (!file)
            {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }
            fprintf(file, "%ld, %d, ", i, n);
            fclose(file);
            
            system("../execution/simple_V2");
            // fprintf(file, "\n");
        }
    }
    // fclose(file);

    return 0;
}