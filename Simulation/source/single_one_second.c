
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP
#define RECORD_EACH_GRID

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"
#include "../include/queue.h"
#include "../include/packets_count.h"

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Missing arguments\n");
        return EXIT_FAILURE;
    }

    char command[MAX_COMMAND_LENGTH];
    clock_t execute_clock = clock();

    int window = atoi(argv[1]);
    double capacity = atof(argv[2]);
    char configuration_path[MAX_PATH_LENGTH];
    strcpy(configuration_path, argv[3]);

    // configuration.h
    configuration config;

    if (ini_parse(configuration_path, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", configuration_path);
        return EXIT_FAILURE;
    }
    long unit = obtainUnit(config);
    // show_configuration(config);
    // printf("data path = %s\n", config.data_path);

    int tenant_number = config.tenant_number;

    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);
    long grids_number = generator.grids_number;
    // generator.grids_number = 2000;

    queue link;
    read_queue(&link, config.data_path, PACKET_LABEL_OVER_CAPACITY_DROPPED);
    // print_link_queue(link);

    packets_count count;
    init_Packets_Count(&count, tenant_number, generator.grids_number);
    read_packets_count(&count, config.data_path);
    // print_packets_count(count);

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);
    read_packets_label(&label, config.data_path);
    // print_packets_label(label);

    queue *shaping = (queue *)malloc(sizeof(queue) * tenant_number);
    read_queues(shaping, config.data_path, PACKET_LABEL_OVER_UPPERBOUND_DROPPED, tenant_number);
    // print_queue(shaping);
    // GCRA *gcras_1 = initializeGCRAs(tenant_number, config.tau_1, config.packet_size);
    // read_gcras(&(gcras_1), tenant_number, config.data_path, 1);

    GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
    read_gcras(&(gcras_2), tenant_number, config.data_path, 2);

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
    // tenant_number = 1;
    // generator.grids_number = 200;
    // printf("generator.grids_number = %ld\n", generator.grids_number);
    for (long grid = 0; grid < generator.grids_number; grid++)
    {
        if (config.traffic_mode == TRAFFIC_MODE_INTERVAL)
            int *packets = packet_generation_uniform(grid, generator.generate_probability, tenant_number);
        else if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY)
            int *packets = packet_generation_naughty(grid, generator.generate_probability, generate.generate_probability_naughty, tenant_number, config.naughty_tenant_number);

        // print_packets(packets, tenant_number);
        long timestamp = window * generator.step_size * generator.grids_number + grid * generator.step_size;
        printf("timestamp = %-ld\n", timestamp);

        int dequeue_count = 0;
        int enqueue_count = 0;
        int enqueue_full = 0;

        int queue_action_count = 0;
        int *queue_action_tenant = (int *)calloc(sizeof(int), tenant_number);

        while (link.dequeue_timestamp <= (double)timestamp)
        {
            // dequeue(&link);
            link.dequeue_timestamp += link.dequeue_interval;
            queue_action_count -= 1;
            // dequeue_count ++;
            // printf("%ld %ld %d\n", link.dequeue_timestamp, timestamp, link.front);
        }

        // if(link.front >0 || link.rear>0)
        // printf("link :front = %d, rear = %d\n", link.front, link.rear);

        for (int tenant = 0; tenant < tenant_number; tenant++)
        {
            // Wheter packet is accepted
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                *(count.count + tenant) += 1;
            else
                goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                int shaping_dequeue_count = 0;
                while ((shaping + tenant)->dequeue_timestamp <= (double)timestamp)
                {
                    // dequeue((queue *)(shaping + tenant));
                    shaping_dequeue_count -= 1;
                    (shaping + tenant)->dequeue_timestamp += (shaping + tenant)->dequeue_interval;
                    // dequeue_count ++;
                    // printf("%ld %ld %d\n", link.dequeue_timestamp, timestamp, link.front);
                }

                if (shaping_dequeue_count > 0)
                    *(packets + tenant) = enqueue((queue *)(shaping + tenant));

                if (shaping_dequeue_count < 0)
                {
                    while (shaping_dequeue_count > -1)
                    {
                        dequeue((queue *)(shaping + tenant));
                        shaping_dequeue_count++;
                    }
                }
                // 51200 281600(0.008342527561)
                // enqueue_count ++;
                // if(*(packets + tenant) == PACKET_LABEL_OVER_UPPERBOUND_DROPPED)
                //     enqueue_full ++;
            }
            else
                goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                long rate_2 = (long)(timestamp - (gcras_2 + tenant)->last_time) * (((double)(config.mean) * unit) / ONE_SECOND_IN_NS);

                // printf("rate = %ld\n", rate_2);
                long x = (long)(gcras_2 + tenant)->x - rate_2;
                // if (x > (gcras_2 + tenant)->tau)
                if (timestamp - (gcras_2 + tenant)->last_time > 4069)
                    printf("lst = %9ld, inter = %6ld, rate_2 = %6ld x= %6ld\n", (gcras_2 + tenant)->last_time, timestamp - (gcras_2 + tenant)->last_time, rate_2, x);
                else
                    printf("lst = %9ld, inter = \x1B[1;31m%6ld\x1B[0m, rate_2 = %6ld x= %6ld\n", (gcras_2 + tenant)->last_time, timestamp - (gcras_2 + tenant)->last_time, rate_2, x);
                // printf("x = %ld, tau_2 = %ld\n", x, config.tau_2);

                if (x > (gcras_2 + tenant)->tau)
                    *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
                else
                {
                    (gcras_2 + tenant)->x = MAX((long)0, x) + (gcras_2 + tenant)->l;
                    (gcras_2 + tenant)->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }
            }
            else
                goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
            {
                // *(packets + tenant) = enqueue(&link);
                // *(queue_action_tenant+tenant) = 1;
                queue_action_count += 1;
                if (queue_action_count > 0)
                {
                    *(packets + tenant) = enqueue(&link);
                    // queue_action_count -= 1;
                }
                // enqueue_count ++;
                // if(*(packets + tenant) == PACKET_LABEL_OVER_CAPACITY_DROPPED)
                //     enqueue_full ++;
            }

            else
                goto RECORD;

        RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET)
            {
                // printf("%d\n", *(packets + tenant));
                label.labels[tenant][*(packets + tenant)] += 1;
            }

#ifdef RECORD_EACH_GRID
            if (tenant == tenant_number - 1)
                fprintf(file, "%d\n", *(packets + tenant));
            else
                fprintf(file, "%d, ", *(packets + tenant));
#endif
            // if (queue_action_count > 0)
            //     printf("[%2d]timestamp = %10ld queue count = %3d queue :%3d\n", tenant, timestamp, queue_action_count, link.rear);
        }

        if (queue_action_count < 0)
        {
            while (queue_action_count > -1)
            {
                dequeue(&link);
                queue_action_count++;
            }
        }
        // if(enqueue_full>0)
        //     printf("link :front = %3d, rear = %3d dequeue_count = %3d, enqueue_count = %3d, enqueue_full = %3d Grid[%4d](%ld)\n", link.front, link.rear, dequeue_count, enqueue_count, enqueue_full, grid, timestamp);
        // printf("link :front = %3d, rear = %3d dequeue_count = %3d, enqueue_count = %3d, enqueue_full = %3d\n", link.front, link.rear, dequeue_count, enqueue_count, enqueue_full);
    }

    record_packets_count(count, config.data_path);
    record_queue(link, config.data_path);
    record_packets_label(label, config.data_path);
    // print_packets_label(label);
    record_queues(shaping, config.data_path, tenant_number);
    // record_gcras(gcras_1, tenant_number, config.data_path, 1);
    record_gcras(gcras_2, tenant_number, config.data_path, 2);

    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    // printf("(%6.4f %), Window = %ld, Time start at %15ld : Execute time : ",
    //        (double)window * 100.0 / config.simulation_time, window, window * generator.step_size * generator.grids_number);
    // printf("%f\n", time_taken);

#ifdef RECORD_EACH_GRID
    fclose(file);
#endif
    return EXIT_SUCCESS;
}