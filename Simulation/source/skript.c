
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

// gcc ./skript.c inih/ini.c -o ../execution/skript -lm
// ../execution/skript [traffic_generation_code]

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
    fprintf(file, "tau, naughty_mean, buffer, naughty_number, regular_loss, naughty_loss\n");
    fclose(file);

    for(int z=0;z<10;z++){
        for(int m =20;m<=80;m+=20){
            config.naughty_tenant_number = m;
            modify_ini_file(CONFIGURATION_PATH, &config);

            for(int b = 100; b<=250; b+= 50){
                for(int n =130;n<=150;n+=10){
                config.naughty_mean = n;
                modify_ini_file(CONFIGURATION_PATH, &config);

                for (long i = 15872; i <= 30208; i += 1024)
                {
                    config.tau = i;
                    modify_ini_file(CONFIGURATION_PATH, &config);
                    file = fopen("../data/naughty.csv", "a");
                    if (!file)
                    {
                        perror("Error opening file");
                        exit(EXIT_FAILURE);
                    }
                    fprintf(file, "%ld, %d, %d, %d, ", i, n, b, m);
                    fclose(file);
                    
                    sprintf(command, "../execution/simple_V2 %d", b);
                    system(command);
                    // fprintf(file, "\n");
                }
            }
            }
            
        }
    }


    // fclose(file);

    return 0;
}