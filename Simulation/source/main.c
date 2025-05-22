// /**
//  * @file main.c
//  * @brief Main program for network traffic simulation
//  * @details Handles configuration loading, directory setup, and capacity calculation
//  */

// /**
//  * @def PRINT_EACH_TIMESTAMP
//  * @brief Debug flag for timestamp printing
//  * @details When defined, enables printing of each timestamp during simulation
//  * @note This is typically used for debugging and performance analysis
//  */
// // #define PRINT_EACH_TIMESTAMP

// /**
//  * @def PRINT_EACH_GRID_PACKET
//  * @brief Debug flag for packet information printing
//  * @details When defined, enables detailed packet information output for each grid
//  * @note This may impact performance when processing large numbers of packets
//  */
// // #define PRINT_EACH_GRID_PACKET

// // #define PRINT_GRID_COUNT          ///< Enable to print the count of grids during execution

// // #define PRINT_REGULAR_AND_NAUGHTY ///< Enable to print regular and naughty data

// /**
//  * @def PRINT_FIRST_INIT_GCRA
//  * @brief Debug macro for printing initial GCRA configuration
//  *
//  * @details When defined, enables the printing of the first GCRA instance's
//  *          configuration after initialization. Useful for debugging and
//  *          verification purposes.
//  */
// // #define PRINT_FIRST_INIT_GCRA

// // // #define PRINT_GCRA_UPDATE ///< Enable to print updates related to GCRA
// // // #define PRINT_EACH_TIMESTAMP ///< Enable to print each timestamp during execution
// // // #define PRINT_EACH_GRID_PACKET ///< Enable to print each grid packet processed
// // #define PRINT_PACKET_COUNTS ///< Enable to print the counts of packets processed
// // #define PRINT_PACKET_LABEL  ///< Enable to print labels for packets processed

// /**
//  * @def PRINT_LINK_DEQUEUE_COUNT
//  * @brief Debug macro for dequeue count monitoring
//  *
//  * @details When defined, enables real-time printing of dequeue
//  *          operations count to standard output. Useful for:
//  *          - Debugging packet processing
//  *          - Performance monitoring
//  *          - System behavior verification
//  *
//  * Output Format:
//  * @code
//  * dequeue_count = XXX
//  * @endcode
//  * where XXX is right-aligned in a 3-character field
//  *
//  * @note The count is displayed with fixed-width formatting (%-3d)
//  *       ensuring consistent output alignment
//  */
// // #define PRINT_LINK_DEQUEUE_COUNT

// // #define RECORD_REGULAR_AND_NAUGHTY_TAU ///< Enable to record tau for regular and naughty data
// // #define RECORD_REGULAR_AND_NAUGHTY_ALL ///< Enable to record all regular and naughty data
// // #define RECORD_AVERAGE_LOSS            ///< Enable to record the average loss during processing
// /**
//  * @def RECORD_PACKETS_SITUATION
//  * @brief Enables packet situation recording functionality
//  *
//  * When defined, this macro enables the creation and initialization of a CSV file
//  * for tracking packet processing statistics throughout the program execution.
//  */
// #define RECORD_PACKETS_SITUATION

#define PRINT_EXECUTION_TIME  ///< Enable timing measurement for execution duration
#define SHOW_CONFIGURATION    ///< Enable display of the current configuration settings
#define REDUCTION             ///< Enable reduction of the INI file (if applicable)
#define CLEAN_DATA_PATH       ///< Enable cleaning and initialization of the data directory
#define PRINT_CAPACITY        ///< Enable display of the calculated network capacity
#define SHOW_TRAFFICGENERATOR ///< Enable the display of the traffic generator's state.

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
// #include "../include/packets_count.h"       ///< Packet counting functions
// #include "../include/GCRA.h"                ///< GCRA (Generic Controlled Rate Algorithm) functions
// #include "../include/link_capacity_queue.h" ///< Link capacity queue management functions

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

    //     /** @brief Structure to store packet counts for each tenant. */
    //     packets_count count;
    //     /**  @brief Initializes packet count tracking. */
    //     init_packets_count(&count, tenant_number, obtain_grids_number(config));

    //     /** @brief Structure to label packets based on their classification. */
    //     packets_label label;
    //     /** @brief Initializes packet labeling structure. */
    //     init_Packets_Label(&label, tenant_number, &count);

    //     meter_queue *meter_queues = init_meter_queues(config);
    //     // printf("dec = %f\n", meter_queues->dequeue_interval);
    //     // printf("a = %d\n", meter_queues->queue.max_buffer);

    //     //     // queue for the exceed upper bound traffic (reservation)

    //     /**
    //      * @brief Initializes an array of GCRA structures for traffic shaping
    //      * @details Creates and initializes an array of GCRA (Generic Cell Rate Algorithm)
    //      *          instances for multiple tenants, with each GCRA controlling the traffic
    //      *          rate and burst characteristics for a specific tenant.
    //      */
    //     GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);

    // #ifdef PRINT_FIRST_INIT_GCRA
    //     /**
    //      * @brief Displays the configuration of the first GCRA instance
    //      *
    //      * @details Prints detailed information about the first GCRA structure including:
    //      *          - Token replenishment interval (tau)
    //      *          - Bucket depth (l)
    //      *          - Current token count (x)
    //      *          - Last update timestamp
    //      * @param[in] gcra Pointer to the first GCRA instance
    //      *
    //      * @note Only the first GCRA instance is displayed to avoid
    //      *       excessive debug output
    //      */
    //     show_GCRA(*(gcras));
    // #endif

    //     //     /**
    //     //      * @brief Initializes a link priority queue.
    //     //      */
    //     //     link_priority_queue link; ///< Link priority queue instance

    //     //     initlink_priority_queue(&link, config.link_queue_buffer, config, capacity); ///< Initialize the link priority queue

    //     //     /** @brief Stores the total number of grids processed. */
    //     //     int grid_counts = 0;
    //     //     int drop_tenant = UNFOUND;

    // #ifdef RECORD_PACKETS_SITUATION
    //     /**
    //      * @brief Initializes the packet situation recording system
    //      *
    //      * @details This code block:
    //      *          1. Creates/overwrites the packet situation CSV file
    //      *          2. Writes the appropriate header
    //      *          3. Prepares the file for subsequent append operations
    //      *
    //      * The file path is constructed as: config.data_path + "/record_packet_situation.csv"
    //      *
    //      * File Format:
    //      * @code
    //      * [Header defined by HEADER_TYPE_PACKET]
    //      * [Subsequent data entries added during program execution]
    //      * @endcode
    //      *
    //      * @param[in] config.data_path Directory where the CSV file will be created
    //      *
    //      * @pre MAX_PATH_LENGTH must be defined
    //      * @pre config.data_path must be valid and writable
    //      * @pre HEADER_TYPE_PACKET must be defined
    //      * @pre write_statistics_header() function must be available
    //      *
    //      * @note Opens file in "w+" mode, which:
    //      *       - Creates a new file if it doesn't exist
    //      *       - Truncates existing file if it exists
    //      *       - Enables both reading and writing
    //      *
    //      * @warning Will terminate program if:
    //      *          - File cannot be opened
    //      *          - Path exceeds MAX_PATH_LENGTH
    //      *
    //      * @see write_statistics_header() for header writing implementation
    //      * @see HEADER_TYPE_PACKET for header structure definition
    //      */
    //     char file_path_packet_situation[MAX_PATH_LENGTH];
    //     sprintf(file_path_packet_situation, "%s/record_packet_situation.csv", config.data_path);

    //     file = fopen(file_path_packet_situation, "w+");
    //     if (!file)
    //     {
    //         perror("Error opening file"); /**< Handle file open errors. */
    //         exit(EXIT_FAILURE);
    //     }

    //     /**
    //      * @brief Writes the statistics header to the newly created file
    //      *
    //      * @return SUCCESS if header is written successfully
    //      * @return FAILURE if an error occurs during writing
    //      *
    //      * @note Prints error message in red if header writing fails
    //      */
    //     if (write_statistics_header(file, HEADER_TYPE_PACKET) == FAILURE)
    //     {
    //         printf(RED_ELOG "write_statistics_header(main) : ERROR\n" RESET);
    //     }
    //     fclose(file); /**< Ensures file is properly closed after initialization */
    // #endif

    //     /**
    //      * @brief Main simulation loop
    //      * @details Iterates through the simulation time, processing traffic events
    //      *          at each time step until reaching the configured end time.
    //      *
    //      * @note Simulation time is converted to nanoseconds using ONE_SECOND_IN_NS
    //      * @see ONE_SECOND_IN_NS
    //      * @see config.simulation_time
    //      */
    //     while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    //     {

    //         /**
    //          * @brief Time advancement block
    //          * @details Updates simulation time and processes dequeue operations
    //          *
    //          * @note The step size (generator.step_size) determines the granularity
    //          *       of the simulation time advancement
    //          * @warning Ensure step_size is appropriate for the simulation scale
    //          */
    //         timestamp += (TIME_TYPE)(generator.step_size);

    //         /**
    //          * @brief Tracks the number of packet dequeue operations
    //          *
    //          * @details Counter variable that maintains a running total of
    //          *          packet dequeue operations performed in the current
    //          *          processing cycle.
    //          */
    //         int link_dequeue_count = 0;

    //         //         while (link.dequeue_timestamp <= (double)timestamp) ///< Loop until the dequeue timestamp exceeds the current timestamp
    //         //         {
    //         //             link.dequeue_timestamp += link.dequeue_interval; ///< Update the dequeue timestamp by the interval
    //         //             dequeue_count += 1;                              ///< Increment the dequeue count
    //         //         }

    //         //         while (cdequeue_timestamp <= (double)timestamp) ///< Loop until the dequeue timestamp exceeds the current timestamp
    //         //         {
    //         //             cdequeue_timestamp += cdequeue_timestamp_step; ///< Update the dequeue timestamp by the interval
    //         //             for (int tenant = 0; tenant < tenant_number; tenant++)
    //         //                 cdequeue((cqueues + tenant));
    //         //         }

    // #ifdef PRINT_LINK_DEQUEUE_COUNT
    //         /**
    //          * @brief Displays current dequeue count
    //          *
    //          * @details Prints the current value of link_dequeue_count
    //          *          to standard output with fixed-width formatting
    //          *
    //          * Format Specifiers:
    //          * - %-3d: Left-aligned, minimum 3 characters wide
    //          *
    //          * Example Output:
    //          * @code
    //          * dequeue_count = 0
    //          * dequeue_count = 12
    //          * dequeue_count = 234
    //          * @endcode
    //          */
    //         printf("dequeue_count = %-3d\n", link_dequeue_count);
    // #endif

    // #ifdef PRINT_EACH_TIMESTAMP
    //         /**
    //          * @brief Debug output for current timestamp
    //          * @details Prints the current simulation time to standard output
    //          * @note Format: timestamp value with floating-point precision
    //          * @note The %-lf format specifier ensures left-aligned output
    //          */
    //         printf("timestamp : %-lf\n", timestamp);
    // #endif
    //         // grid_counts++;

    //         // Packets generation
    //         int *packets = packet_generation_configuration(generator, config);

    // #ifdef PRINT_EACH_GRID_PACKET
    //         /**
    //          * @brief Print detailed packet information for all tenants
    //          * @details Outputs the current state of packets for each tenant in the grid
    //          *
    //          * @param[in] packets Array of packet information for each tenant
    //          * @param[in] tenant_number Total number of tenants in the simulation
    //          *
    //          * @see print_packets
    //          * @note This debug output may be verbose for large tenant numbers
    //          */
    //         print_packets(packets, config.tenant_number);
    // #endif

    // #ifdef RECORD_PACKETS_SITUATION
    //         /**
    //          * @brief Records detailed packet processing statistics per grid
    //          *
    //          * @details Records comprehensive packet situation data including:
    //          *          - Packet processing status
    //          *          - Dequeue operations count
    //          *          - Grid-specific metrics
    //          *          - Temporal information
    //          *
    //          * The function writes data to the previously initialized CSV file
    //          * (record_packet_situation.csv) with the following information:
    //          * - Current packet states and statistics
    //          * - Grid-based processing metrics
    //          * - Dequeue operation counts
    //          * - System configuration parameters
    //          *
    //          * @param[in] packets             Pointer to the packet data structure
    //          * @param[in] link_dequeue_count  Number of dequeue operations performed
    //          * @param[in] config              System configuration parameters
    //          *
    //          * @pre RECORD_PACKETS_SITUATION must be defined
    //          * @pre CSV file must be properly initialized
    //          * @pre packets pointer must be valid
    //          * @pre config must be properly initialized
    //          *
    //          * @note This recording operation occurs after packet processing
    //          *       and dequeue operations are completed
    //          *
    //          * @warning May impact performance if called frequently
    //          *          Consider the I/O overhead in performance-critical scenarios
    //          *
    //          * @see init_packets_count() for packet structure initialization
    //          * @see write_statistics_header() for CSV file header format
    //          */
    //         record_packet_situation_agrid(packets, link_dequeue_count, config);
    // #endif

    //         for (int tenant = 0; tenant < tenant_number; tenant++)
    //         {

    //             /**
    //              * @brief Increments the packet count for a specific tenant based on packet label.
    //              *
    //              * This code checks if the packet for the specified tenant is of the
    //              * type PACKET_LABEL_ACCEPT. If it is, the count for that tenant is
    //              * incremented. If not, the loop continues without any further action.
    //              */
    //             if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
    //             {
    //                 *(count.count + tenant) += 1; ///< Increment the count for the tenant
    //             }
    //             else
    //                 continue; ///< Skip to the next iteration if there is no relevant packet

    //             //             // Upper bound exceeded traffic operation(reservation)

    //             //             if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
    //             //             {
    //             //                 if (cenqueue((cqueues + tenant)) == 1)
    //             //                     *(packets + tenant) == PACKET_LABEL_ACCEPT;
    //             //             }
    //             //             else
    //             //             {
    //             //                 label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] += 1;
    //             //                 continue; ///< Skip to the next iteration if there is no relevant packet
    //             //             }

    //             //             /**
    //             //              * @brief Processes packets through the GCRA algorithm.
    //             //              *
    //             //              * This code checks if the packet for the specified tenant is of the
    //             //              * type PACKET_LABEL_ACCEPT. If it is, the packet undergoes an update
    //             //              * using the GCRA algorithm based on the current timestamp and tenant's
    //             //              * GCRA instance.
    //             //              */
    //             //             if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Check if the packet label is ACCEPT for the tenant
    //             //             {
    //             //                 *(packets + tenant) = gcra_update(timestamp, (gcras + tenant), config); ///< Update the packet using GCRA
    //             //             }

    //             //             /**
    //             //              * @brief Updates the statistics for packets labeled as GCRA.
    //             //              *
    //             //              * This code checks if the packet for the specified tenant is labeled
    //             //              * as PACKET_LABEL_GCRA_LABELED. If it is, the corresponding statistic
    //             //              * for that label is incremented in the labels structure.
    //             //              */
    //             //             // Statistic for the number of packets labeled as GCRA
    //             //             if (*(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is labeled as GCRA
    //             //             {
    //             //                 label.labels[tenant][PACKET_LABEL_GCRA_LABELED] += 1; ///< Increment the GCRA labeled packet count for the tenant
    //             //             }

    //             //             /**
    //             //              * @brief Enqueues packets into the link queue based on their labels.
    //             //              *
    //             //              * This code checks if the packet for the specified tenant is either
    //             //              * labeled as PACKET_LABEL_ACCEPT or PACKET_LABEL_GCRA_LABELED. If
    //             //              * the packet is labeled as ACCEPT, it processes the enqueue operation
    //             //              * for the link queue, handling potential drops due to capacity limits.
    //             //              */
    //             //             // Link queue enqueue
    //             //             if (*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is ACCEPT or GCRA labeled
    //             //             {
    //             //                 if (*(packets + tenant) == PACKET_LABEL_ACCEPT) ///< Further check if the packet is ACCEPT
    //             //                 {
    //             //                     if (dequeue_count > 0) ///< Check if there are packets to dequeue
    //             //                     {
    //             //                         int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
    //             //                         if (index != UNFOUND)                              ///< Check if a valid index was returned
    //             //                             label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
    //             //                         dequeue_count -= 1;                                ///< Decrease the dequeue count
    //             //                     }

    //             //                     enqueue(&link, ALPHA, tenant, &drop_tenant); ///< Enqueue the ACCEPT packet into the link queue

    //             //                     if (drop_tenant != UNFOUND)                                                 ///< Check if a packet was dropped
    //             //                         if (drop_tenant != tenant)                                              ///< If the dropped packet is not from the current tenant
    //             //                             label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the other tenant
    //             //                         else
    //             //                             label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the current tenant
    //             //                 }
    //             //             }

    //             //             /**
    //             //              * @brief Enqueues packets labeled as GCRA into the link queue.
    //             //              *
    //             //              * This code checks if the packet for the specified tenant is labeled
    //             //              * as PACKET_LABEL_GCRA_LABELED. If it is, the packet is enqueued
    //             //              * into the link queue, and any drops due to capacity limits are
    //             //              * recorded.
    //             //              */
    //             //             if (*(packets + tenant) == PACKET_LABEL_GCRA_LABELED) ///< Check if the packet is labeled as GCRA
    //             //             {
    //             //                 enqueue(&link, BETA, tenant, &drop_tenant); ///< Enqueue the GCRA labeled packet into the link queue

    //             //                 if (drop_tenant != UNFOUND)                                             ///< Check if a packet was dropped
    //             //                     label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1; ///< Increment the drop count for the tenant whose packet was dropped
    //             //             }
    //             //         }

    //             //         /**
    //             //          * @brief Dequeues packets from the link queue and updates statistics.
    //             //          *
    //             //          * This code processes packets in the link queue while there are
    //             //          * packets available to dequeue. For each dequeued packet, the
    //             //          * corresponding statistic for PACKET_LABEL_ACCEPT is incremented.
    //             //          */
    //             //         while (dequeue_count > 0) ///< Continue processing while there are packets to dequeue
    //             //         {
    //             //             int index = dequeue(&link);                        ///< Dequeue a packet from the link queue
    //             //             if (index != UNFOUND)                              ///< Check if a valid index was returned
    //             //                 label.labels[index][PACKET_LABEL_ACCEPT] += 1; ///< Increment the count for ACCEPT labeled packets
    //             //             dequeue_count -= 1;                                ///< Decrease the dequeue count
    //         }
    //         //         // show_LinkQueue(&link);
    //     }

    //     // #ifdef PRINT_GRID_COUNT
    //     //     printf("grid numbers= %d\n", grid_counts);
    //     // #endif

    //     // #ifdef PRINT_PACKET_COUNTS
    //     //     show_packets_count(count);
    //     // #endif

    //     // #ifdef PRINT_PACKET_LABEL
    //     //     show_packets_label(label);
    //     // #endif

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
    //     free_packets_count(&count);
    //     free_packets_label(&label);

    return EXIT_SUCCESS;
}