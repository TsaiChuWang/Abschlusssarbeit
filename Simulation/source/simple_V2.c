
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP
// #define RECORD_EACH_GRID

#define PRINT_GRID_COUNT
// #define PRINT_GCRA
#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/queue.h"
#include "../include/packets_count.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"

#define CONFIGURATION_PATH "../configuration/simple_V2.ini"

// gcc ./simple_V2.c inih/ini.c -o ../execution/simple_V2 -lm
// ../execution/simple_V2

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];
    clock_t execute_clock = clock();

    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
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
    // printf("capacity : %f bps\n", capacity * config.unit);

    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);

    int tenant_number = config.tenant_number;
    long grids_number = generator.grids_number;
    TIME_TYPE timestamp = (TIME_TYPE)0;

    int grid_counts = 0;
    int GCRAdrop = 0;

    packets_count count;
    init_Packets_Count(&count, tenant_number, obtain_grids_number(config.packet_size));

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);

    queue *shaping = initQueues(config.upper_queue_buffer, config, (double)(config.mean + config.standard_deviation), PACKET_LABEL_OVER_UPPERBOUND_DROPPED, tenant_number);

    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);

    // int linkmb = atoi(argv[1]);
    int linkmb = 50;
    link_capacity_queue link;
    initLinkQueue(&link, linkmb, config, capacity);

#ifdef RECORD_EACH_GRID
    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }
#endif

    int max_x = 0;
    // printf("%lf\n", (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS));
    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {
        timestamp += (TIME_TYPE)(generator.step_size);
        // printf("timestamp : %-lf\n", timestamp);
        grid_counts++;

        int dequeue_count = 0;
        while (link.dequeue_timestamp <= (double)timestamp)
        {
            link.dequeue_timestamp += link.dequeue_interval;
            dequeue_count += 1;
        }
        // printf("queue count = %d\n", dequeue_count);

        int *packets;
        // printf("%d\n", config.traffic_mode);
        if (config.traffic_mode == TRAFFIC_MODE_INTERVAL)
            packets = packet_generation_uniform(grid_counts, generator.generate_probability, tenant_number);
        else if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY)
            packets = packet_generation_naughty(grid_counts, generator.generate_probability, generator.generate_probability_naughty, tenant_number, config.naughty_tenant_number);
        else if (config.traffic_mode == TRAFFIC_MODE_FULL)
            packets = packet_generation_full(tenant_number);
        else if(config.traffic_mode == TRAFFIC_MODE_DENSITY){
            // printf("%d\n", atoi(argv[1]));
            packets = packet_generation_density(tenant_number, grid_counts, atoi(argv[1]));
            // printf("a\n");
        }   
            
        // print_packets(packets, tenant_number);
        int packet_time_count = 0;

        for (int tenant = 0; tenant < tenant_number; tenant++)
        {
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                packet_time_count += 1;
                *(count.count + tenant) += 1;
            }

            else
                continue;

            // if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            // {
            //     int shaping_dequeue_count = 0;
            //     while ((shaping + tenant)->dequeue_timestamp <= (double)timestamp)
            //     {
            //         shaping_dequeue_count -= 1;
            //         (shaping + tenant)->dequeue_timestamp += (shaping + tenant)->dequeue_interval;
            //     }
            //     if (shaping_dequeue_count < 0)
            //         *(packets + tenant) = enqueue((queue *)(shaping + tenant));

            //     if (shaping_dequeue_count < 0)
            //         while (shaping_dequeue_count < -1)
            //         {
            //             dequeue((queue *)(shaping + tenant));
            //             shaping_dequeue_count++;
            //         }
            // }
            // else
            //     goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                long rate = (long)(timestamp - (gcras + tenant)->last_time) * (((double)(config.mean) * config.unit) / ONE_SECOND_IN_NS);

                long x = (long)((gcras + tenant)->x - rate);

                if (x > max_x)
                    max_x = x;

#ifdef PRINT_GCRA
                if (timestamp - (gcras + tenant)->last_time > 4069)
                    printf("[%2d]lst = %9lf, time = %-7f, inter = %7lf, rate = %6ld x= %6ld\n", tenant, (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x);
                else
                    printf("[%2d]lst = %9lf, time = %-7f, inter = \x1B[1;31m%6lf\x1B[0m, rate = %6ld x= %6ld\n", tenant, (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x);
                printf("x = %ld, tau = %ld %d\n", x, (gcras + tenant)->tau, x > (gcras + tenant)->tau);
#endif
                if (x > (gcras + tenant)->tau)
                    *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
                else
                {
                    (gcras + tenant)->x = MAX((long)0, x) + (gcras + tenant)->l;
                    (gcras + tenant)->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }
            }

            if (*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
            {
                GCRAdrop += 1;
                label.labels[tenant][*(packets + tenant)] += 1;
            }

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
            {

                if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                {
                    *(packets + tenant) = enqueueLink(&link, tenant, ALPHA);
                    if (dequeue_count > 0)
                    {
                        dequeueLink(&link);
                        dequeue_count -= 1;
                    }
                }

                if (*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
                {
                    *(packets + tenant) = enqueueLink(&link, tenant, BETA);
                }
            }
            // printf("%d\n", *(packets + tenant));
        RECORD:
            if (*(packets + tenant) >= PACKET_LABEL_ACCEPT)
                label.labels[tenant][*(packets + tenant)] += 1;
        }

        // printf("front = %2d rear = %2d front = %2d rear = %2d\n", link.alpha_front,
        //        link.alpha_rear,
        //        link.beta_front,
        //        link.beta_rear);

        while (dequeue_count > 0)
        {
            dequeueLink(&link);
            dequeue_count -= 1;
        }

        // printf("packet_time_count = %d\n", packet_time_count);
        // printf("front = %2d rear = %2d front = %2d rear = %2d\n", link.alpha_front,
        //        link.alpha_rear,
        //        link.beta_front,
        //        link.beta_rear);
    }

    // printf("GCRA drop = %d(%d)\n", GCRAdrop, GCRAdrop - 8192025);
    // printf("maxx = %d\n", max_x);
#ifdef RECORD_EACH_GRID
    fclose(file);
#endif

#ifdef PRINT_GRID_COUNT
    printf("Gird Count = %d\n", grid_counts);
#endif

    print_packets_label(label);
    // printf("%ld\n", config.tau);
    // print_naughty(label, config.naughty_tenant_number);
    // printf("%2d : (%-7f) %\n", linkmb, (double)label.labels[tenant_number - 1][3] * 100.0 / (label.labels[tenant_number - 1][0] + label.labels[tenant_number - 1][3]));
    // for (int tenant = 0; tenant < tenant_number; tenant++)
    //     printf("%2d : %-7lf % (%-7lf)\n", tenant, (double)label.labels[tenant][0], (double)*(count.count + tenant));

    // printf("%2d : %-7lf % (%-7lf)\n", tenant, (double)label.labels[tenant][3] / (label.labels[tenant][0] + label.labels[tenant][1] + label.labels[tenant][2] + label.labels[tenant][3]), (double)*(count.count + tenant) / grid_counts);
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    // printf("Execute time : %f\n", time_taken);

    return EXIT_SUCCESS;
}
