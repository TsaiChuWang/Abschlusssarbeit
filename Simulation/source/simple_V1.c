
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP
#define RECORD_EACH_GRID

// #define PRINT_GRID_COUNT
// #define PRINT_GCRA
#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/queue.h"
#include "../include/packets_count.h"
#include "../include/GCRA.h"


#define CONFIGURATION_PATH "../configuration/simple_V1.ini"

// gcc ./simple_V1.c inih/ini.c -o ../execution/simple_V1 -lm
// ../execution/simple_V1

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];
    clock_t execute_clock = clock();


    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0){
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }
    // show_configuration(config);

    // Clean
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);
    
    traffic_generator generator = initializeTrafficGenerator(config);

    int tenant_number = config.tenant_number;
    long grids_number = generator.grids_number;
    TIME_TYPE timestamp = (TIME_TYPE)0;

    int grid_counts = 0;

    packets_count count;
    init_Packets_Count(&count, tenant_number, obtain_grids_number(config.packet_size));

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);

    queue *shaping = initQueues(config.upper_queue_buffer, config, (double)(config.mean + config.standard_deviation), PACKET_LABEL_OVER_UPPERBOUND_DROPPED, tenant_number);
    
    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);

#ifdef RECORD_EACH_GRID
    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);
    FILE *file = fopen(filename, "w");
    if (file == NULL){
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }
#endif

    while(timestamp<=(TIME_TYPE)(config.simulation_time*ONE_SECOND_IN_NS)){
        timestamp += (TIME_TYPE)(generator.step_size);
        // printf("timestamp : %-lf\n", timestamp);

        int *packets = packet_generation_uniform(grid_counts, generator.generate_probability, tenant_number);

        for (int tenant = 0; tenant < tenant_number; tenant++){
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                *(count.count + tenant) += 1;
            else goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                int shaping_dequeue_count = 0;
                while ((shaping + tenant)->dequeue_timestamp <= (double)timestamp){
                    shaping_dequeue_count -= 1;
                    (shaping + tenant)->dequeue_timestamp += (shaping + tenant)->dequeue_interval;
                }
                if (shaping_dequeue_count < 0)
                    *(packets + tenant) = enqueue((queue *)(shaping + tenant));

                if (shaping_dequeue_count < 0)
                    while (shaping_dequeue_count < -1){
                        dequeue((queue *)(shaping + tenant));
                        shaping_dequeue_count++;
                    }
            }
            else goto RECORD;     

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                long rate = (long)(timestamp - (gcras + tenant)->last_time) * (((double)(config.mean) * config.unit) / ONE_SECOND_IN_NS);

                long x = (long)((gcras + tenant)->x - rate);

#ifdef PRINT_GCRA
                if (timestamp - (gcras + tenant)->last_time > 4069)
                    printf("lst = %9lf, time = %-7f, inter = %7lf, rate = %6ld x= %6ld\n", (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x);
                else
                    printf("lst = %9lf, time = %-7f, inter = \x1B[1;31m%6lf\x1B[0m, rate = %6ld x= %6ld\n", (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x);
                printf("x = %ld, tau = %ld\n", x, config.tau);
#endif
                if (x > (gcras + tenant)->tau)
                    *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
                else{
                    (gcras + tenant)->x = MAX((long)0, x) + (gcras + tenant)->l;
                    (gcras + tenant)->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }
            }
RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET){
                // printf("%d\n", *(packets + tenant));
                label.labels[tenant][*(packets + tenant)] += 1;
            }

        }

        grid_counts ++;
    }

#ifdef RECORD_EACH_GRID
    fclose(file);
#endif

#ifdef PRINT_GRID_COUNT
    printf("Gird Count = %d\n", grid_counts);
#endif

    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("%d : %f\n", tenant, (double)label.labels[tenant][2]/grid_counts);

    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f\n", time_taken);

    return EXIT_SUCCESS;
}