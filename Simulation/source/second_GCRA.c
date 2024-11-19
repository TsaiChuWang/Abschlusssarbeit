
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"


// #define STORED_PACKET_GENERATION_PATH "../data/test/packet_generation"
// #define STORED_TIMESTAMP_PATH "../data/test/timestamp"

// gcc ./second_GCRA.c inih/ini.c -o ../execution/second_GCRA -lm
// ../execution/second_GCRA [configuration path] [identifier] [data folder]
// ../execution/second_GCRA ../configuration/simple_V1.ini 0 ../data/test/timestamp/ 
#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[]){
    configuration config;

    if (ini_parse(argv[1], handler, &config) < 0) {
        printf("Can't load configuration\"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }

    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%stenant_%d.csv", argv[3], atoi(argv[2]));
    size_t size = 0;
    
    long long *timestamps = read_csv(data_path, &size);
    printf("Timestamps (%zu entries):\n", size);

    GCRA gcra = initializeGCRA(config.tau_2, config.packet_size*2);
    int* label = (int*)calloc(size, sizeof(int));
    long drop = 0;

    double coefficient = (double)(config.mean)*MBPS/ONE_SECOND_IN_NS;
    printf("%lf\n", coefficient);

    // printf("%ld\n", config.tau_2);
    for(long index = 0;index<size;index++){
      long long timestamp = *(timestamps+index);
      long long x = gcra.x - (long long)((timestamp - gcra.last_time)*coefficient);
      // printf("%ld %ld %ld %ld\n", gcra.x, x, timestamp, gcra.last_time);
      if(x>config.tau_2){
        *(label+index) = PACKET_LABEL_GCRA_DROPPED;
        drop ++;
      }
      else{
          gcra.x = MAX((long)0, x)+gcra.l;
          gcra.last_time = timestamp;
          *(label+index) = PACKET_LABEL_ACCEPT;        
      }
    }

    printf("drop    = %-ld\n", drop);
    printf("loss   = %-f\n", (double)drop*100.0/size);
    return EXIT_SUCCESS;
}