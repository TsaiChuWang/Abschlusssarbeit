
#define UNIFORM_DISTRIBUTION 0
#define BURSTY_ALL 1
#define BURSTY_REGULAR 2
#define BURSTY_NAUGHTY 3

#define INTERVAL 0
#define NAUGHTY 1

#define REDUCTION

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/main.ini"

// gcc ./skript.c inih/ini.c -o ../execution/skript -lm
// ../execution/skript

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

    system("gcc ./main.c inih/ini.c -o ../execution/main -lm");

    char file_average_loss_path[MAX_PATH_LENGTH];
    sprintf(file_average_loss_path, "%s/record_average_loss.csv", config.data_path);
    // printf("%s(../data/main/record_average_loss.csv)\n", file_average_loss_path);

    FILE *file = fopen(file_average_loss_path, "w+");
    if (!file)
    {
        perror("Error opening file"); /**< Handle file open errors. */
        exit(EXIT_FAILURE);
    }
    fprintf(file, RECORD_AVERAGE_LOSS_HEADER);
    fclose(file);

    reduction_inif_file(CONFIGURATION_PATH);
    switch (atoi(argv[1]))
    {
    case UNIFORM_DISTRIBUTION:
        if (atoi(argv[2]) == INTERVAL)
        {
            long step = 32;
            for (long tau = 0; tau <= 51200; tau += step)
            {
                config.tau = tau;
                modify_ini_file(CONFIGURATION_PATH, &config);

                system("../execution/main");
            }

            system("python3 ../python/average_loss.py");
        }
        break;

    default:
        long step = 32;
        for (long tau = 0; tau <= 51200; tau += step)
        {
            config.tau = tau;
            modify_ini_file(CONFIGURATION_PATH, &config);

            system("python3 ../python/average_loss.py");
        }
        break;
    }

    return 0;
}