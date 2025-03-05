
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP
// #define RECORD_EACH_GRID

// #define PRINT_GRID_COUNT
// #define PRINT_TRAFFIC_MODE
// #define PRINT_GCRA
// #define PRINT_MAX_X

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/queue.h"
#include "../include/packets_count.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"

#define CONFIGURATION_PATH "../configuration/simple_V3.ini"

// gcc ./simple_V3.c ./inih/ini.c -o ../execution/simple_V3 -lm
// ../execution/simple_V3

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

    // Calculate the capacity
    double capacity = 13486.768514886677;
    // printf("capacity : %f bps\n", capacity * config.unit);

    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);

    // Float variable
    int tenant_number = config.tenant_number;
    long grids_number = generator.grids_number;
    TIME_TYPE timestamp = (TIME_TYPE)0;

    // Statistical variables
    int grid_counts = 0;
    int GCRAdrop = 0;
#ifdef PRINT_MAX_X
    int max_x = 0;
#endif

    packets_count count;
    init_Packets_Count(&count, tenant_number, obtain_grids_number(config.packet_size));

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);

    queue *shaping = initQueues(config.upper_queue_buffer, config, (double)(config.mean + config.standard_deviation), PACKET_LABEL_OVER_UPPERBOUND_DROPPED, tenant_number);

    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);

    link_capacity_queue link;
    initLinkQueue(&link, config, capacity);
    int drop_tenant = -1;

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

    //     int t = 0;

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
        // printf("dequeue_count = %d\n", dequeue_count);

        int *packets;
        switch (config.traffic_mode)
        {
        case TRAFFIC_MODE_INTERVAL:
            packets = packet_generation_uniform(grid_counts, generator.generate_probability, tenant_number);
#ifdef PRINT_TRAFFIC_MODE
            printf("Traffic Mode : INTERVAL (All the tenants are regular tenants.)\n");
#endif
            break;

        case TRAFFIC_MODE_NAUGHTY:
            packets = packet_generation_naughty(grid_counts, generator.generate_probability, generator.generate_probability_naughty, tenant_number, config.naughty_tenant_number);
#ifdef PRINT_TRAFFIC_MODE
            printf("Traffic Mode : NAUGHTY (There are specified tenants are naughty tenants with different statistical properties.)\n");
#endif
            break;
        case TRAFFIC_MODE_FULL:
            packets = packet_generation_full(tenant_number);
#ifdef PRINT_TRAFFIC_MODE
            printf("Traffic Mode : FULL (All the tenants send upper bound traffics.)\n");
#endif
            break;
        case TRAFFIC_MODE_DENSITY:
            packets = packet_generation_density(tenant_number, grid_counts, atoi(argv[1]));
#ifdef PRINT_TRAFFIC_MODE
            printf("Traffic Mode : DENSITY (Not commended operation.)\n");
#endif
            break;
        default:
            printf(RED_ELOG "Traffic Mode Code does not exist.\n" RESET);
            exit(FAILED);
            break;
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

#ifdef PRINT_MAX_X
                if (x > max_x)
                    max_x = x;
#endif

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
                    *(packets + tenant) = enqueueLink(&link, tenant, ALPHA, &drop_tenant);
                    if (dequeue_count > 0)
                    {
                        dequeueLink(&link);
                        dequeue_count -= 1;
                    }
                }

                if (*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
                {
                    *(packets + tenant) = enqueueLink(&link, tenant, BETA, &drop_tenant);
                }
            }

            // printf("teant = %d drop_tenant = %d\n", tenant, drop_tenant);
            if (drop_tenant != tenant && drop_tenant < tenant_number)
                label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;

            // RECORD:
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                if (*(packets + tenant) == PACKET_LABEL_OVER_CAPACITY_DROPPED)
                {
                    label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;
                }
                else
                    label.labels[tenant][*(packets + tenant)] += 1;
        }

        //         // printf("front = %2d rear = %2d front = %2d rear = %2d\n", link.alpha_front,
        //         //        link.alpha_rear,
        //         //        link.beta_front,
        //         //        link.beta_rear);

        //         int t = -1;
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
#ifdef PRINT_MAX_X
    printf("max_x = %d\n", max_x);
#endif
    // #ifdef RECORD_EACH_GRID
    //     fclose(file);
    // #endif

#ifdef PRINT_GRID_COUNT
    printf("Gird Count = %d\n", grid_counts);
#endif

    print_packets_label(label);
    calculate_tau_variation(label, config.naughty_tenant_number);
    //     // printf("%2d : (%-7f) %\n", linkmb, (double)label.labels[tenant_number - 1][3] * 100.0 / (label.labels[tenant_number - 1][0] + label.labels[tenant_number - 1][3]));
    //     // for (int tenant = 0; tenant < tenant_number; tenant++)
    //     //     printf("%2d : %-7lf % (%-7lf)\n", tenant, (double)label.labels[tenant][0], (double)*(count.count + tenant));

    //     // printf("%2d : %-7lf % (%-7lf)\n", tenant, (double)label.labels[tenant][3] / (label.labels[tenant][0] + label.labels[tenant][1] + label.labels[tenant][2] + label.labels[tenant][3]), (double)*(count.count + tenant) / grid_counts);
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f\n", time_taken);

    return EXIT_SUCCESS;
}
