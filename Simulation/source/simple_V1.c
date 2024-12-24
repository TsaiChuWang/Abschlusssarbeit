
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
#include "../include/link_capacity_queue.h"

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
    
    double capacity = obtain_capacity();
    printf("capacity : %f bps\n", capacity * config.unit);

    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);

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

    link_capacity_queue link;
    initLinkQueue(&link, tenant_number, config, capacity);

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

        int queue_action_count = 0;
        while (link.dequeue_timestamp <= (double)timestamp){
            link.dequeue_timestamp += link.dequeue_interval;
            queue_action_count -= 1;
        }
        // printf("queue count = %d\n", queue_action_count);

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
            }else goto RECORD;  

            if(*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_DROPPED){
                if(*(packets + tenant) == PACKET_LABEL_ACCEPT){
                    queue_action_count += 1;
                    if (queue_action_count > 0)
                        *(packets + tenant) = enqueueLink(&link, tenant);
                    else goto RECORD;  
                }

                if(*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED){
                    link.beta ++;
                    link.beta_ptr_label[link.beta] = tenant;
                    continue;
                }
            }
RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET){
                // printf("%d\n", *(packets + tenant));
                label.labels[tenant][*(packets + tenant)] += 1;
            }

        }

        for(int beta = 0; beta <= link.beta; beta++){
            queue_action_count += 1;
            int tenant = link.beta_ptr_label[beta];
            if(queue_action_count > 0)
                label.labels[tenant][enqueueLink(&link, tenant)] += 1;
            else label.labels[tenant][0] += 1;
        }

        link.beta = -1;
        memset(link.beta_ptr_label, -1, link.max_buffer);

        grid_counts ++;
    }

#ifdef RECORD_EACH_GRID
    fclose(file);
#endif

#ifdef PRINT_GRID_COUNT
    printf("Gird Count = %d\n", grid_counts);
#endif

    for (int tenant = 0; tenant < tenant_number; tenant++)
        printf("%2d : %-7lf %\n", tenant, (double)label.labels[tenant][3]/(label.labels[tenant][0]+label.labels[tenant][1]+label.labels[tenant][2]+label.labels[tenant][3]));
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f\n", time_taken);

    return EXIT_SUCCESS;
}