
#define REDUCTION

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"

#define CONFIGURATION_PATH "../configuration/simple_V1.ini"

// gcc ./simple_V1.c inih/ini.c -o ../execution/simple_V1
// ../execution/simple_V1 

int main(int argc, char *argv[]){
    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
        printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    show_configuration(config);

    config.packet_size = 512;
    modify_ini_file(CONFIGURATION_PATH, &config);

#ifdef REDUCTION
    reduction_inif_file(CONFIGURATION_PATH);
#endif

    // capacity.py
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "python3 ../python/capacity.py %s %d", CONFIGURATION_PATH, 3);
    system(command);

    free(command);
    return EXIT_SUCCESS;
}