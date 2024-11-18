#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"

// gcc ./traffic_generation_tenant.c inih/ini.c -o ../execution/traffic_generation_tenant -lm
// ../execution/traffic_generation_tenant [configuration path] [identifier] [record packets generation path] [record timestamp path]

int main(int argc, char *argv[]){

    configuration config;

    if (ini_parse(argv[1], handler, &config) < 0) {
        printf("Can't load configuration\"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }

    int identifier = atoi(argv[2]);
    tenant t = initialize_tenant(identifier, config.simulation_time);
    tenant_packet_generation(&t, config.mean, config.standard_deviation, config.naughty_mean, config.naughty_standard_deviation);
    translate_band_to_packets(&t, MBPS, config.packet_size);

#ifdef RECORD_PACKETS_GENERATION
    record_generated_packets(&t, argv[3]);
#endif

    timestamp_translation(&t);
#ifdef RECORD_TIMESTAMP
    record_timestamps(&t, argv[4]);
#endif   

  return EXIT_SUCCESS;
}