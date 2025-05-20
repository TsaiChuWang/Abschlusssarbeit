/**
 * @brief Defines for controlling debug output and data recording.
 */

// #define CLEAN_DATA_PATH ///< Enable to clean data path before processing

#define PRINT_GRID_COUNT          ///< Enable to print the count of grids during execution
#define PRINT_EXECUTION_TIME      ///< Enable to print the execution time of processes
#define PRINT_CAPACITY            ///< Enable to print the capacity information
#define PRINT_REGULAR_AND_NAUGHTY ///< Enable to print regular and naughty data
// #define PRINT_FIRST_INIT_GCRA ///< Enable to print information on the first GCRA initialization
// #define PRINT_GCRA_UPDATE ///< Enable to print updates related to GCRA
// #define PRINT_EACH_TIMESTAMP ///< Enable to print each timestamp during execution
// #define PRINT_EACH_GRID_PACKET ///< Enable to print each grid packet processed
#define PRINT_PACKET_COUNTS ///< Enable to print the counts of packets processed
#define PRINT_PACKET_LABEL  ///< Enable to print labels for packets processed
// #define PRINT_DEQUEUE_COUNT ///< Enable to print the count of dequeued items

#define RECORD_REGULAR_AND_NAUGHTY_TAU ///< Enable to record tau for regular and naughty data
#define RECORD_REGULAR_AND_NAUGHTY_ALL ///< Enable to record all regular and naughty data
#define RECORD_AVERAGE_LOSS            ///< Enable to record the average loss during processing
#define RECORD_PACKETS_SITUATION       ///< Macro for recording packet situations.

/**
 * @file main.c
 * @brief Main entry point for the traffic generation application.
 *
 * This file includes necessary headers and defines the main function
 * for the traffic generation application. It sets up configurations
 * and handles the execution flow.
 */

#include "../include/general.h"             ///< General utility functions and definitions
#include "./inih/ini.h"                     ///< INI file parsing library
#include "../include/configuration.h"       ///< Configuration handling functions
#include "../include/traffic_generation.h"  ///< Traffic generation functions and definitions
#include "../include/packets_count.h"       ///< Packet counting functions
#include "../include/GCRA.h"                ///< GCRA (Generic Controlled Rate Algorithm) functions
#include "../include/link_capacity_queue.h" ///< Link capacity queue management functions

#define CONFIGURATION_PATH "../configuration/main.ini" ///< Path to the main configuration file

#define NAME "main" ///< Name of the application

// gcc ./main.c inih/ini.c -o ../execution/main -lm ///< Command to compile the application
// ../execution/main///< Command to execute the compiled application with traffic generation code

int main(int argc, char *argv[])
{
    /**
     * @brief Main execution block that loads the configuration and optionally cleans the data directory.
     */

    char command[MAX_COMMAND_LENGTH];

#ifdef PRINT_EXECUTION_TIME
    clock_t execute_clock = clock(); ///< Stores the execution start time for performance measurement.
#endif

    // configuration.h
    configuration config; ///< Holds configuration settings loaded from the INI file.

    /** @brief Parses the configuration file. */
    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }
    show_configuration(config); // Uncomment if you need to debug configuration settings.

#ifdef CLEAN_DATA_PATH
    /**
     * @brief Cleans and initializes the data directory.
     * @details Deletes the existing data directory and recreates it along with a subdirectory for images.
     */
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s/images", config.data_path);
    system(command);
#endif

    /** @brief Calculates the link capacity. */
    sprintf(command, "python3 ../python/capacity.py %s 0", CONFIGURATION_PATH);
    system(command);
    double capacity = obtain_capacity();

#ifdef PRINT_CAPACITY
    /**  @brief Prints the calculated network capacity. */
    printf("capacity : %f bps\n", capacity);
#endif

    /** @brief Initializes the random seed for random number generation.*/
    srand48(time(NULL));

    /** @brief Initializes the traffic generator based on the configuration. */
    double r = atof(argv[1]);
    traffic_generator generator = initializeTrafficGenerator(config, r);
    showTrafficGenerator(generator); ///< Uncomment to display generator details.

    /** @brief Number of tenants in the simulation.(Float) */
    int tenant_number = config.tenant_number;
    /** @brief Total number of grid time slots in the simulation.(Float) */
    long grids_number = generator.grids_number;

    /** @brief The simulation timestamp. */
    TIME_TYPE timestamp = (TIME_TYPE)0;

    /** @brief Structure to store packet counts for each tenant. */
    packets_count count;
    /**  @brief Initializes packet count tracking. */
    init_packets_count(&count, tenant_number, obtain_grids_number(config));

    /** @brief Structure to label packets based on their classification. */
    packets_label label;
    /** @brief Initializes packet labeling structure. */
    init_Packets_Label(&label, tenant_number, &count);

    double cdequeue_timestamp = 0;
    double cdequeue_timestamp_step = (TIME_TYPE)(config.packet_size *
                                                 (double)ONE_SECOND_IN_NS / ((config.mean + config.standard_deviation) * config.unit));
    // printf("dec = %f\n", cdequeue_timestamp_step);
    circular_queue *cqueues = init_circular_queues(config);
    // printf("a = %d\n", cqueues->size);

    // queue for the exceed upper bound traffic (reservation)

    /** @brief Array of GCRA (Generic Cell Rate Algorithm) structures for traffic shaping. */
    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);
#ifdef PRINT_FIRST_INIT_GCRA
    /** @brief Prints the details of the first initialized GCRA instance. */
    show_GCRA(*(gcras));
#endif

    /**
     * @brief Initializes a link priority queue.
     */
    link_priority_queue link; ///< Link priority queue instance

    initlink_priority_queue(&link, config.link_queue_buffer, config, capacity); ///< Initialize the link priority queue

    /** @brief Stores the total number of grids processed. */
    int grid_counts = 0;
    int drop_tenant = UNFOUND;

#ifdef RECORD_PACKETS_SITUATION
    /**
     * @brief Creates and opens a CSV file for recording packet situations.
     *
     * This block of code defines the file path for the packet situation
     * CSV file, opens it for writing, and handles any errors that occur
     * during file operations.
     */
    char file_path_packet_situation[MAX_PATH_LENGTH];
    sprintf(file_path_packet_situation, "%s/record_packet_situation.csv", config.data_path);

    FILE *file = fopen(file_path_packet_situation, "w+");
    if (!file)
    {
        perror("Error opening file"); /**< Handle file open errors. */
        exit(EXIT_FAILURE);
    }

    fprintf(file, RECORD_PACKET_SITUATION_HEADER); // Placeholder for writing data to the file.
    fclose(file);
#endif

    /**
     * @brief Main loop for simulating the traffic over time.
     * @details This loop runs until the timestamp exceeds the total simulation time.
     *          The timestamp is incremented by the step size at each iteration.
     */
    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {

        /**
         * @brief Updates the timestamp and counts the dequeue operations.
         *
         * This section of code updates the current timestamp based on the
         * generator's step size and counts how many times items can be
         * dequeued from the link priority queue based on the dequeue interval.
         */

        timestamp += (TIME_TYPE)(generator.step_size); ///< Update the timestamp by the generator's step size

        // Count the dequeue times
        int dequeue_count = 0; ///< Initialize the dequeue count

        while (link.dequeue_timestamp <= (double)timestamp) ///< Loop until the dequeue timestamp exceeds the current timestamp
        {
            link.dequeue_timestamp += link.dequeue_interval; ///< Update the dequeue timestamp by the interval
            dequeue_count += 1;                              ///< Increment the dequeue count
        }

        while (cdequeue_timestamp <= (double)timestamp) ///< Loop until the dequeue timestamp exceeds the current timestamp
        {
            cdequeue_timestamp += cdequeue_timestamp_step; ///< Update the dequeue timestamp by the interval
            for (int tenant = 0; tenant < tenant_number; tenant++)
                cdequeue((cqueues + tenant));
        }
#ifdef PRINT_DEQUEUE_COUNT
        printf("dequeue_count = %d\n", dequeue_count);
#endif

#ifdef PRINT_EACH_TIMESTAMP
        printf("timestamp : %-lf\n", timestamp);
#endif
        grid_counts++;

        // Packets generation
        int *packets = packet_generation_configuration(generator, config);

#ifdef PRINT_EACH_GRID_PACKET
        print_packets(packets, config.tenant_number);
#endif

#ifdef RECORD_PACKETS_SITUATION
        /**
         * @brief Records the packet situation if RECORD_PACKETS_SITUATION is defined.
         */
        record_packet_situation_agrid(packets, dequeue_count, config);
#endif

        for (int tenant = 0; tenant < tenant_number; tenant++)
        {

            /**
             * @brief Increments the packet count for a specific tenant based on packet label.
             *
             * This code checks if the packet for the specified tenant is of the
             * type PACKET_LABEL_ACCEPT. If it is, the count for that tenant is
             * incremented. If not, the loop continues without any further action.
             */
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
            {
                *(count.count + tenant) += 1; ///< Increment the count for the tenant
            }
            else
                continue; ///< Skip to the next iteration if there is no relevant packet

            // Upper bound exceeded traffic operation(reservation)

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
            {
                if (cenqueue((cqueues + tenant)) == 1)
                    *(packets + tenant) == PACKET_LABEL_ACCEPT;
            }
            else
            {
                label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] += 1;
                continue; ///< Skip to the next iteration if there is no relevant packet
            }

            /**
             * @brief Processes packets through the GCRA algorithm.
             *
             * This code checks if the packet for the specified tenant is of the
             * type PACKET_LABEL_ACCEPT. If it is, the packet undergoes an update
             * using the GCRA algorithm based on the current timestamp and tenant's
             * GCRA instance.
             */
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
            {
                *(packets + tenant) = gcra_update(timestamp, (gcras + tenant), config); ///< Update the packet using GCRA
            }

            /**
             * @brief Updates the statistics for packets labeled as GCRA.
             *
             * This code checks if the packet for the specified tenant is labeled
             * as PACKET_LABEL_GCRA_LABELED. If it is, the corresponding statistic
             * for that label is incremented in the labels structure.
             */
            // Statistic for the number of packets labeled as GCRA
            if (*(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is labeled as GCRA
            {
                label.labels[tenant][PACKET_LABEL_GCRA_LABELED] += 1; ///< Increment the GCRA labeled packet count for the tenant
            }

            /**
             * @brief Enqueues packets into the link queue based on their labels.
             *
             * This code checks if the packet for the specified tenant is either
             * labeled as PACKET_LABEL_ACCEPT or PACKET_LABEL_GCRA_LABELED. If
             * the packet is labeled as ACCEPT, it processes the enqueue operation
             * for the link queue, handling potential drops due to capacity limits.
             */
            // Link queue enqueue
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is ACCEPT or GCRA labeled
            {
                if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Further check if the packet is ACCEPT
                {
                    if (dequeue_count > 0) ///< Check if there are packets to dequeue
                    {
                        int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
                        if (index != UNFOUND)                              ///< Check if a valid index was returned
                            label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
                        dequeue_count -= 1;                                ///< Decrease the dequeue count
                    }

                    enqueue(&link, ALPHA, tenant, &drop_tenant); ///< Enqueue the ACCEPT packet into the link queue

                    if (drop_tenant != UNFOUND)                                                 ///< Check if a packet was dropped
                        if (drop_tenant != tenant)                                              ///< If the dropped packet is not from the current tenant
                            label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the other tenant
                        else
                            label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the current tenant
                }
            }

            /**
             * @brief Enqueues packets labeled as GCRA into the link queue.
             *
             * This code checks if the packet for the specified tenant is labeled
             * as PACKET_LABEL_GCRA_LABELED. If it is, the packet is enqueued
             * into the link queue, and any drops due to capacity limits are
             * recorded.
             */
            if (*(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is labeled as GCRA
            {
                enqueue(&link, BETA, tenant, &drop_tenant); ///< Enqueue the GCRA labeled packet into the link queue

                if (drop_tenant != UNFOUND)                                             ///< Check if a packet was dropped
                    label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the tenant whose packet was dropped
            }
        }

        /**
         * @brief Dequeues packets from the link queue and updates statistics.
         *
         * This code processes packets in the link queue while there are
         * packets available to dequeue. For each dequeued packet, the
         * corresponding statistic for PACKET_LABEL_ACCEPT is incremented.
         */
        while (dequeue_count > 0) ///< Continue processing while there are packets to dequeue
        {
            int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
            if (index != UNFOUND)                              ///< Check if a valid index was returned
                label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
            dequeue_count -= 1;                                ///< Decrease the dequeue count
        }
        // show_LinkQueue(&link);
    }

#ifdef PRINT_GRID_COUNT
    printf("grid numbers= %d\n", grid_counts);
#endif

#ifdef PRINT_PACKET_COUNTS
    show_packets_count(count);
#endif

#ifdef PRINT_PACKET_LABEL
    show_packets_label(label);
#endif

#ifdef PRINT_REGULAR_AND_NAUGHTY
    if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    {
        print_regular_and_naughty(label, config);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_TAU
    if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    {
        record_regular_and_naughty_tau(label, config);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_ALL
    if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    {
        record_regular_and_naughty_all(label, config);
    }
#endif

#ifdef RECORD_AVERAGE_LOSS
    record_average_loss(label, config);
#endif

#ifdef RECORD_PACKETS_SITUATION
    switch (config.traffic_mode)
    {
    case TRAFFIC_MODE_INTERVAL:
        system("python3 ../python/packet_situation.py 0");
        break;

    default:
        system("python3 ../python/packet_situation.py 0");
        break;
    }

#endif

#ifdef PRINT_EXECUTION_TIME
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f s\n", time_taken);
#endif

    return EXIT_SUCCESS;
}