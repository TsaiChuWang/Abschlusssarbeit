
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/link_capacity_queue.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/packets_count.h"

// #define CONFIGURATION_PATH "../configuration/simple_V1.ini"
#define CONFIGURATION_PATH "../configuration/simple_V2.ini"

#define NAME "test"

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

    link_capacity_queue link;
    initLinkQueue(&link, config, 100);

    int code = 0;
    int drop_tenant = -1;

    printf("\n");
    code = enqueueLink(&link, 1, ALPHA, &drop_tenant);
    printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    printLinkQueue(&link);

    printf("\n");
    code = enqueueLink(&link, 2, ALPHA, &drop_tenant);
    printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    printLinkQueue(&link);

    printf("\n");
    code = enqueueLink(&link, 3, ALPHA, &drop_tenant);
    printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    printLinkQueue(&link);

    printf("\n");
    code = enqueueLink(&link, 4, ALPHA, &drop_tenant);
    printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    printLinkQueue(&link);

    // printf("\n");
    // code = enqueueLink(&link, 1, ALPHA, &drop_tenant);
    // printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    // printLinkQueue(&link);

    // printf("\n");
    // code = enqueueLink(&link, 2, ALPHA, &drop_tenant);
    // printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    // printLinkQueue(&link);

    // printf("\n");
    // code = enqueueLink(&link, 3, BETA, &drop_tenant);
    // printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    // printLinkQueue(&link);

    // printf("\n");
    // code = enqueueLink(&link, 4, ALPHA, &drop_tenant);
    // printf("code = %d,  drop_tenant = %d\n", code, drop_tenant);
    // printLinkQueue(&link);
    return EXIT_SUCCESS;
}