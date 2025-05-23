// /**
//  * @file main.c
//  * @brief Main program for network traffic simulation
//  * @details Handles configuration loading, directory setup, and capacity calculation
//  */

// // #define PRINT_GRID_COUNT          ///< Enable to print the count of grids during execution

// // #define PRINT_REGULAR_AND_NAUGHTY ///< Enable to print regular and naughty data

// // //

// // #define RECORD_REGULAR_AND_NAUGHTY_TAU ///< Enable to record tau for regular and naughty data
// // #define RECORD_REGULAR_AND_NAUGHTY_ALL ///< Enable to record all regular and naughty data
// // #define RECORD_AVERAGE_LOSS            ///< Enable to record the average loss during processing

#define PRINT_EXECUTION_TIME  ///< Enable timing measurement for execution duration
#define SHOW_CONFIGURATION    ///< Enable display of the current configuration settings
#define REDUCTION             ///< Enable reduction of the INI file (if applicable)
#define CLEAN_DATA_PATH       ///< Enable cleaning and initialization of the data directory
#define PRINT_CAPACITY        ///< Enable display of the calculated network capacity
#define SHOW_TRAFFICGENERATOR ///< Enable the display of the traffic generator's state.
// #define PRINT_EACH_TIMESTAMP      ///< Enable printing of each timestamp.
// #define PRINT_METER_DEQUEUE_COUNT ///< Enable printing of the meter dequeue count.
// #define PRINT_EACH_GRID_PACKET ///< Enable printing of each grid packet.
// #define PRINT_ORDER ///< Enable printing of the tenant order.
#define PRINT_PACKET_COUNTS ///< Enable printing of packet counts.
#define PRINT_PACKET_LABEL  ///< Enable printing of packet labels.
// #define PRINT_FIRST_INIT_GCRA ///< Enable printing of the first initialized GCRA.
// #define PRINT_LINK_DEQUEUE_COUNT ///< Enable printing of the link dequeue count.
// #define PRINT_GCRA_UPDATE ///< Enable to print updates related to GCRA

#define RECORD_PACKETS_SITUATION ///< Enable recording of packet situations.

// /**
//  * @file main.c
//  * @brief Main entry point for the traffic generation application.
//  *
//  * This file includes necessary headers and defines the main function
//  * for the traffic generation application. It sets up configurations
//  * and handles the execution flow.
//  */

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/packets_count.h"       ///< Packet counting functions
#include "../include/GCRA.h"                ///< GCRA (Generic Controlled Rate Algorithm) functions
#include "../include/link_capacity_queue.h" ///< Link capacity queue management functions

#define CONFIGURATION_PATH "../configuration/main.ini" ///< Path to the main configuration file

/** @brief Compilation command
 *  @code{.sh}
 *  gcc ./main.c inih/ini.c -o ../execution/main -lm
 *  @endcode
 */

/** @brief Execution command
 *  @code{.sh}
 *  ../execution/main
 *  @endcode
 */

int main(int argc, char *argv[])
{
    char configuration_path[MAX_PATH_LENGTH];
    if (argc < 2)
        strcpy(configuration_path, CONFIGURATION_PATH);
    else
        strcpy(configuration_path, argv[1]);

    /** @brief Buffer for system commands */
    char *command = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
    memset(command, '\0', MAX_COMMAND_LENGTH * sizeof(char));

    FILE *file; ///< File pointer for potential file operations

#ifdef PRINT_EXECUTION_TIME
    clock_t execute_clock = clock(); ///< Start time measurement for performance tracking
#endif

    configuration config; ///< Configuration structure to hold settings

    /** @brief Load and parse the configuration file.
     *
     * This function reads the specified INI file and populates the
     * configuration structure. If the file cannot be loaded or parsed,
     * an error message is displayed and the program exits.
     */
    if (ini_parse(configuration_path, handler, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path);
        return EXIT_FAILURE;
    }

#ifdef SHOW_CONFIGURATION
    /** @brief Display current configuration if enabled.
     *
     * This section shows the loaded configuration settings to the user,
     * which can be useful for debugging or verification purposes.
     */
    show_configuration(config);
#endif

#ifdef CLEAN_DATA_PATH
    /**
     * @brief Clean and initialize the data directory.
     *
     * This process involves removing the existing data directory and
     * creating a new one, including a subdirectory for images. This
     * ensures that old data does not interfere with new operations.
     *
     * @details The following operations are performed:
     * - Remove the existing data directory specified in the configuration.
     * - Create a new data directory.
     * - Create an images subdirectory within the data directory.
     */
    sprintf(command, "rm -r %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s", config.data_path);
    system(command);
    sprintf(command, "mkdir %s/images", config.data_path);
    system(command);
#endif

    /**
     * @brief Calculate network capacity.
     *
     * This section executes a Python script to calculate the network
     * capacity based on the configuration file provided. The output of
     * the script is then processed to retrieve the calculated capacity.
     *
     * @details The Python script is invoked with the configuration file
     * as an argument. The result is stored in a variable for further use.
     */
    sprintf(command, "python3 ../python/capacity.py %s 0", configuration_path);
    system(command);
    double capacity = obtain_capacity(); ///< Variable to hold the calculated capacity

#ifdef PRINT_CAPACITY
    /** @brief Display the calculated capacity.
     *
     * This section outputs the calculated network capacity to the console.
     * The capacity is displayed in bits per second (bps).
     */
    printf("capacity : %f bps\n", capacity);
#endif

    srand48(time(NULL)); ///< Seed the random number generator with the current time.

    traffic_generator generator = initializeTrafficGenerator(config); ///< Initialize the traffic generator with the provided configuration.

    int tenant_number = config.tenant_number;   ///< Retrieve the number of tenants from the configuration.
    long grids_number = generator.grids_number; ///< Get the number of grids from the traffic generator.

    TIME_TYPE timestamp = (TIME_TYPE)0; ///< Initialize the timestamp variable to zero.

#ifdef SHOW_TRAFFICGENERATOR
    showTrafficGenerator(generator); ///< Display the current state and configuration of the traffic generator.
#endif

    /** @brief Structure to store packet counts for each tenant. */
    packets_count count;
    /**  @brief Initializes packet count tracking. */
    init_packets_count(&count, tenant_number, obtain_grids_number(config));

    /** @brief Structure to label packets based on their classification. */
    packets_label label;
    /** @brief Initializes packet labeling structure. */
    init_Packets_Label(&label, tenant_number, &count);

    // Initialize an array of meter queues based on the provided configuration.
    meter_queue *meter_queues = init_meter_queues(config);

    /**
     * @brief Initializes an array of GCRA structures for traffic shaping
     * @details Creates and initializes an array of GCRA (Generic Cell Rate Algorithm)
     *          instances for multiple tenants, with each GCRA controlling the traffic
     *          rate and burst characteristics for a specific tenant.
     */
    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);

#ifdef PRINT_FIRST_INIT_GCRA
    show_GCRA(*(gcras)); ///< Display the details of the first initialized GCRA instance.
#endif

    /**
     * @brief Initializes a link priority queue.
     */
    link_priority_queue link;                                                   ///< Link priority queue instance
    initlink_priority_queue(&link, config.link_queue_buffer, config, capacity); ///< Initialize the link priority queue

    /** @brief Stores the total number of grids processed. */
    int grid_counts = 0;
    int drop_tenant = UNFOUND;

#ifdef RECORD_PACKETS_SITUATION
    char file_path_packet_situation[MAX_PATH_LENGTH];                                        ///< Path for the packet situation file.
    sprintf(file_path_packet_situation, "%s/record_packet_situation.csv", config.data_path); ///< Format the file path.

    // Open the file for writing (create if it doesn't exist).
    file = fopen(file_path_packet_situation, "w+");
    if (!file) ///< Check if the file opened successfully.
    {
        perror("Error opening file"); /**< Handle file open errors. */
        exit(EXIT_FAILURE);           ///< Exit if the file cannot be opened.
    }

    // Write the header for the packet statistics.
    if (write_statistics_header(file, HEADER_TYPE_PACKET) == FAILURE)
    {
        printf(RED_ELOG "write_statistics_header(main) : ERROR\n" RESET); ///< Print error message if writing the header fails.
    }

    fclose(file); /**< Ensure the file is properly closed after initialization. */
#endif

    /**
     * @brief Main simulation loop
     * @details Iterates through the simulation time, processing traffic events
     *          at each time step until reaching the configured end time.
     *
     * @note Simulation time is converted to nanoseconds using ONE_SECOND_IN_NS
     * @see ONE_SECOND_IN_NS
     * @see config.simulation_time
     */
    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {
        // Increment the timestamp by the step size defined in the generator.
        timestamp += (TIME_TYPE)(generator.step_size);
#ifdef PRINT_EACH_TIMESTAMP
        printf("timestamp : %-lf\n", timestamp); ///< Print the current timestamp with a formatted output.
#endif

        // Initialize the count of dequeued meters.
        int meter_dequeue_count = 0;

        // Loop until the dequeue timestamp exceeds the current timestamp.
        while ((meter_queues)->dequeue_timestamp <= (double)timestamp)
        {
            (meter_queues)->dequeue_timestamp += (meter_queues)->dequeue_interval; ///< Update the dequeue timestamp by the dequeue interval.
            meter_dequeue_count += 1;                                              ///< Increment the count of dequeued meters.
        }
#ifdef PRINT_METER_DEQUEUE_COUNT
        printf("meter_dequeue_count = %d \n", meter_dequeue_count); ///< Print the total meter dequeue count.
#endif

        // Allocate memory for an array to store the dequeue counts for each tenant.
        int *meter_dequeue_counts = (int *)malloc(tenant_number * sizeof(int));
        for (int index = 0; index < tenant_number; index++)
        {
            *(meter_dequeue_counts + index) = meter_dequeue_count; ///< Store the meter dequeue count for the current tenant.
#ifdef PRINT_METER_DEQUEUE_COUNT
            printf("%d \n", *(meter_dequeue_counts + index)); ///< Print the dequeue count for the current tenant.
#endif
        }
#ifdef PRINT_METER_DEQUEUE_COUNT
        print_equals_line(); ///< Print a line of equal signs for formatting.
#endif

        int link_dequeue_count = 0; ///< Initialize the count of link dequeues to zero.
        // Loop until the dequeue timestamp exceeds the current timestamp.
        while (link.dequeue_timestamp <= (double)timestamp)
        {
            link.dequeue_timestamp += link.dequeue_interval; ///< Update the dequeue timestamp by the interval.
            link_dequeue_count += 1;                         ///< Increment the dequeue count for each dequeue operation.
        }
#ifdef PRINT_LINK_DEQUEUE_COUNT
        printf("link dequeue_count = %-3d\n", link_dequeue_count); ///< Print the total number of link dequeues.
#endif

        grid_counts++; ///< Increment the grid counts.

        // Generate packets based on the configuration provided by the generator.
        int *packets = packet_generation_configuration(generator, config);
#ifdef PRINT_EACH_GRID_PACKET
        print_packets(packets, config.tenant_number); ///< Print the generated packets for each tenant.
#endif

#ifdef RECORD_PACKETS_SITUATION
        record_packet_situation_agrid(packets, link_dequeue_count, config);
#endif

        // Generate a shuffled array of tenant indices.
        int *order = generateShuffledArray(tenant_number);

        // Iterate over each tenant index in the shuffled array.
        for (int index = 0; index < tenant_number; index++)
        {
#ifdef PRINT_ORDER
            printf("%d ", *(order + index)); ///< Print the current tenant index.
#endif
            int tenant = *(order + index); ///< Retrieve the tenant index from the shuffled array.

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

            /**
             * @brief Check for upper bound exceeded traffic operation.
             *
             * This section of code verifies whether the traffic operation for a specific tenant
             * exceeds the upper bound of accepted packets. It specifically checks if the packet
             * label for the tenant is set to ACCEPT. If the packet label is ACCEPT, the code
             * proceeds to manage the meter dequeue and enqueue operations accordingly.
             *
             * The following checks are performed:
             * - If the packet label is ACCEPT, it checks if there are packets available to
             *   dequeue from the meter.
             * - If there are packets to dequeue, it performs the dequeue operation and
             *   decrements the count of available packets for the tenant.
             * - After dequeuing, it attempts to enqueue a packet back into the meter queue.
             * - If the enqueue operation is successful, the packet label remains ACCEPT.
             * - If the enqueue operation fails, it increments the drop count for packets that
             *   exceed the upper bound and continues to the next tenant.
             *
             * If the packet label is not ACCEPT, the code skips to the next iteration without
             * performing any operations.
             */
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant.
            {
                if (*(meter_dequeue_counts + tenant) > 0) ///< Check if there are packets to dequeue for the tenant.
                {
                    meter_dequeue((meter_queues + tenant)); ///< Dequeue a packet from the meter queue for the tenant.
                    *(meter_dequeue_counts + tenant) -= 1;  ///< Decrement the count of dequeued packets for the tenant.
                }

                // Enqueue a packet if the meter queue is ready.
                if (meter_enqueue((meter_queues + tenant)) == QUEUE_READY)
                {
                    *(packets + tenant) = PACKET_LABEL_ACCEPT; ///< Set the packet label to ACCEPT for the tenant.
                }
                else
                {
                    label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] += 1; ///< Increment the drop count for exceeding upper bound.
                    continue;                                                        ///< Skip to the next iteration if enqueueing fails.
                }
            }
            else
            {
                continue; ///< Skip to the next iteration if there is no relevant packet.
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
                    if (link_dequeue_count > 0) ///< Check if there are packets to dequeue
                    {
                        int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
                        if (index != UNFOUND)                              ///< Check if a valid index was returned
                            label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
                        link_dequeue_count -= 1;                           ///< Decrease the dequeue count
                    }

                    enqueue(&link, ALPHA, tenant, &drop_tenant); ///< Enqueue the ACCEPT packet into the link queue

                    if (drop_tenant != UNFOUND)                                                 ///< Check if a packet was dropped
                        if (drop_tenant != tenant)                                              ///< If the dropped packet is not from the current tenant
                            label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the other tenant
                        else
                            label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the current tenant
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
        }

        // Iterate over each tenant.
        for (int tenant = 0; tenant < tenant_number; tenant++)
        {
            // Dequeue packets from the meter queue while there are packets to dequeue for the tenant.
            while (*(meter_dequeue_counts + tenant) > 0)
            {
                meter_dequeue((meter_queues + tenant)); ///< Dequeue a packet from the meter queue for the tenant.
                *(meter_dequeue_counts + tenant) -= 1;  ///< Decrement the count of dequeued packets for the tenant.
            }
        }

        /**
         * @brief Dequeues packets from the link queue and updates statistics.
         *
         * This code processes packets in the link queue while there are
         * packets available to dequeue. For each dequeued packet, the
         * corresponding statistic for PACKET_LABEL_ACCEPT is incremented.
         */
        while (link_dequeue_count > 0) ///< Continue processing while there are packets to dequeue
        {
            int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
            if (index != UNFOUND)                              ///< Check if a valid index was returned
                label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
            link_dequeue_count -= 1;                           ///< Decrease the dequeue count
        }

#ifdef PRINT_ORDER
        printf("\n");        ///< Print a newline for better output formatting.
        print_equals_line(); ///< Print a line of equal signs for visual separation in the output.
#endif
    }

    //     // #ifdef PRINT_GRID_COUNT
    //     //     printf("grid numbers= %d\n", grid_counts);
    //     // #endif

#ifdef PRINT_PACKET_COUNTS
    show_packets_count(count); ///< Display the count of packets.
#endif

#ifdef PRINT_PACKET_LABEL
    show_packets_label(label); ///< Display the labels of packets.
#endif

    //     // #ifdef PRINT_REGULAR_AND_NAUGHTY
    //     //     if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    //     //     {
    //     //         print_regular_and_naughty(label, config);
    //     //     }
    //     // #endif

    //     // #ifdef RECORD_REGULAR_AND_NAUGHTY_TAU
    //     //     if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    //     //     {
    //     //         record_regular_and_naughty_tau(label, config);
    //     //     }
    //     // #endif

    //     // #ifdef RECORD_REGULAR_AND_NAUGHTY_ALL
    //     //     if (config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL)
    //     //     {
    //     //         record_regular_and_naughty_all(label, config);
    //     //     }
    //     // #endif

    //     // #ifdef RECORD_AVERAGE_LOSS
    //     //     record_average_loss(label, config);
    //     // #endif

    //     // #ifdef RECORD_PACKETS_SITUATION
    //     //     switch (config.traffic_mode)
    //     //     {
    //     //     case TRAFFIC_MODE_INTERVAL:
    //     //         system("python3 ../python/packet_situation.py 0");
    //     //         break;

    //     //     default:
    //     //         system("python3 ../python/packet_situation.py 0");
    //     //         break;
    //     //     }

    //     // #endif

    // #ifdef PRINT_EXECUTION_TIME
    //     /**
    //      * @brief Calculate and display total execution time
    //      * @details Measures the elapsed time since program start and converts it to seconds
    //      *
    //      * The calculation follows these steps:
    //      * 1. Subtract start time from current time to get clock ticks
    //      * 2. Convert clock ticks to seconds by dividing by CLOCKS_PER_SEC
    //      * 3. Display the result in seconds
    //      *
    //      * @note CLOCKS_PER_SEC is system-dependent and defined in time.h
    //      * @note Precision may vary depending on the system's clock resolution
    //      */
    //     execute_clock = clock() - execute_clock;
    //     double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    //     printf("Execute time : %f s\n", time_taken);
    // #endif

    free(command);
    freeTrafficGenerator(&generator);
    free(meter_queues);
    free(gcras);
    free_packets_count(&count);
    free_packets_label(&label);

    return EXIT_SUCCESS;
}