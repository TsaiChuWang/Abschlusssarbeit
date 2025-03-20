
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#define ALL_UNIFORM_DISTRIBUTION 1

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/packets_count.h"

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

#ifdef ALL_UNIFORM_DISTRIBUTION
    #define CONFIGURATION_PATH "../configuration/all_unifrorm_distribution.ini"
    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    system("gcc ./all_unifrorm_distribution.c inih/ini.c -o ../execution/all_unifrorm_distribution -lm");
    system("rm ../data/all_unifrorm_distribution.csv");
    FILE *file = fopen("../data/all_unifrorm_distribution.csv", "a");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "tau, regular_loss, naughty_loss\n");
    fclose(file);

    long step = 256;
    for (long tau = 0; tau <= 153600000; tau += step){
        config.tau = tau;
        modify_ini_file(CONFIGURATION_PATH, &config);

        file = fopen("../data/all_unifrorm_distribution.csv", "a");
        if (!file)
        {
            perror("Error opening file");
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%ld, ", tau);
        fclose(file);

        system("../execution/all_unifrorm_distribution");

        if(tau >= step*10)
            step*= 2;
    }
    
    system("python3 ../python/all_unifrorm_distribution.py");
#endif
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

    // system("gcc ./simple_V2.c inih/ini.c -o ../execution/simple_V2 -lm");

    // system("rm ../data/naughty.csv");
    // FILE *file = fopen("../data/naughty.csv", "a");
    // if (!file)
    // {
    //     perror("Error opening file");
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(file, "tau, naughty_mean, buffer, naughty_number, regular_loss, naughty_loss\n");
    // fclose(file);

    // for (int retimes = 0; retimes < 10; retimes++)
    //     for (int naughty_tenant_number = 80; naughty_tenant_number >= 20; naughty_tenant_number -= 20)
    //         for (int link_queue_buffer = 250; link_queue_buffer >= 100; link_queue_buffer -= 50)
    //             for (int naughty_mean = 150; naughty_mean >= 130; naughty_mean -= 10)
    //                 for (long tau = 30208; tau >= 15872; tau -= 1024)
    //                 {
    //                     config.naughty_tenant_number = naughty_tenant_number;
    //                     config.link_queue_buffer = link_queue_buffer;
    //                     config.naughty_mean = naughty_mean;
    //                     config.tau = tau;
    //                     modify_ini_file(CONFIGURATION_PATH, &config);

    //                     file = fopen("../data/naughty.csv", "a");
    //                     if (!file)
    //                     {
    //                         perror("Error opening file");
    //                         exit(EXIT_FAILURE);
    //                     }
    //                     fprintf(file, "%ld, %d, %d, %d, ", tau, naughty_mean, link_queue_buffer, naughty_tenant_number);
    //                     fclose(file);
    //                     system("../execution/simple_V2");
    //                 }

    return 0;
}