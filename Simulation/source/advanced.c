#define PRINT_EXECUTION_TIME ///< Enable timing measurement for execution duration
// #define SHOW_CONFIGURATION   ///< Enable display of the current configuration settings
// #define SHOW_CSV_CONFIGURATION // Uncomment this line to enable the display
#define PRINT_CAPACITY        ///< Enable display of the calculated network capacity
// #define PRINT_EACH_TIMESTAMP      ///< Enable printing of each timestamp.
// #define PRINT_LINK_DEQUEUE_COUNT ///< Enable printing of the link dequeue count.
// #define PRINT_ORDER ///< Enable printing of the tenant order.
#define PRINT_PACKET_COUNT

#include "../include/general.h"       ///< Include general definitions and declarations.
#include "./inih/ini.h"               ///< Include INI file handling library.
#include "../include/configuration.h" ///< Include configuration management definitions.

#include "../include/traffic_generation.h"  ///< Include traffic generation functions and definitions.
#include "../include/GCRA.h"                ///< Include Generic Cell Rate Algorithm (GCRA) definitions.
#include "../include/link_capacity_queue.h" ///< Include link capacity and queue management definitions.
#include "../include/single_flow.h"

#define CONFIGURATION_PATH "../configuration/advanced.ini" ///< Path to the main configuration file
#define CSV_PATH "../configuration/csv/advanced.csv"       ///< Path to the main csv file

// #define TEST_AREA

/** @brief Compilation command
 *  @code{.sh}
 *  gcc ./advanced.c inih/ini.c -o ../execution/advanced -lm
 *  @endcode
 */

/** @brief Execution command
 *  @code{.sh}
 *  ../execution/advanced
 *  @endcode
 */

int main(int argc, char *argv[])
{
    char configuration_path[MAX_PATH_LENGTH];
    // Check if a configuration path is provided as a command-line argument
    if (argc < 2)
        strcpy(configuration_path, CONFIGURATION_PATH); // Use default configuration path
    else
        strcpy(configuration_path, argv[1]); // Use the provided configuration path

#ifdef PRINT_EXECUTION_TIME
    clock_t execute_clock = clock(); ///< Start time measurement for performance tracking
#endif

    common_configuration config; ///< Configuration structure to hold settings
    srand48(time(NULL)); ///< Seed the random number generator with the current time.

    /** @brief Load and parse the configuration file.
     *
     * This function reads the specified INI file and populates the
     * configuration structure. If the file cannot be loaded or parsed,
     * an error message is displayed and the program exits.
     */
    if (ini_parse(configuration_path, handler_common_configuration, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path);
        return EXIT_FAILURE;
    }

#ifdef SHOW_CONFIGURATION
    show_configuration_common_configuration(config); /** @brief Display current configuration if enabled. */
#endif

#ifdef TEST_AREA
    // test_configuration_h_common_configuration(configuration_path);
    test_csv_function(CSV_PATH, &config);
#endif

    csv_configuration csv_config = create_csv_configuration(config.csv_data_path, &config);
    modify_ini_file_common_configuration(configuration_path, &config);

#ifdef SHOW_CSV_CONFIGURATION
    show_csv_configuration(csv_config); // Display the CSV configuration
#endif

    /** @brief Buffer for system commands */
    char *command = (char *)malloc(MAX_COMMAND_LENGTH * sizeof(char));
    memset(command, '\0', MAX_COMMAND_LENGTH * sizeof(char));

    sprintf(command, "python3 ../python/capacity_advanced.py %s %s 0", config.csv_data_path, configuration_path);
    system(command);
    double capacity = obtain_capacity_advanced("../data/capacity_advanced.txt") * config.ratio; ///< Variable to hold the calculated capacity

#ifdef PRINT_CAPACITY
    /** @brief Display the calculated capacity.
     *
     * This section outputs the calculated network capacity to the console.
     * The capacity is displayed in bits per second (bps).
     */
    printf("capacity : %f bps\n", capacity);
#endif

    // Initialize flow index and allocate memory for flows array
    int flow_index = 0;
    single_flow *flows = (single_flow *)malloc(config.tenant_number * sizeof(single_flow)); // Allocate memory for single_flow structures

    // Loop through each kind of configuration in csv_config
    for (int i = 0; i < csv_config.kind_number; i++)
    {
        // Initialize flows for each row configuration until the end index is reached
        while (flow_index <= (csv_config.rows + i)->end_index)
        {
            init_single_flow((flows + flow_index), (csv_config.rows + i), &flow_index, config); // Initialize a single flow
            // show_single_flow(*(flows + flow_index - 1)); // Uncomment to display the flow details
        }
    }

    link_priority_queue link;
    initlink_priority_queue_advanced(&link, config.link_queue_buffer, config, capacity, (flows)->packet_size); // now doesn't support different packet size

    TIME_TYPE timestamp = (TIME_TYPE)0;
    TIME_TYPE time_per_packet = (double)ONE_SECOND_IN_NS / config.input_rate;
    TIME_TYPE step_size = (TIME_TYPE)((flows)->packet_size * time_per_packet); // now doesn't support different packet size
    int drop_tenant = UNFOUND;

    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {
        // Increment the timestamp by the step size defined in the generator.
        timestamp += (TIME_TYPE)(step_size);
#ifdef PRINT_EACH_TIMESTAMP
        printf("timestamp : %-lf\n", timestamp); ///< Print the current timestamp with a formatted output.
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

        // Generate a shuffled array of tenant indices.
        int *order = generateShuffledArray(config.tenant_number);

        // Iterate over each tenant index in the shuffled array.
        for (int index = 0; index < config.tenant_number; index++)
        {
#ifdef PRINT_ORDER
            printf("%d ", *(order + index)); ///< Print the current tenant index.
#endif
            int tenant = *(order + index); ///< Retrieve the tenant index from the shuffled array.

            int packet = three_update((flows+tenant), timestamp, config);

            if(packet == PACKET_LABEL_NO_PACKET || packet == PACKET_LABEL_OVER_UPPERBOUND_DROPPED)
                continue;

            if (link_dequeue_count > 0) ///< Check if there are packets to dequeue
            {
                int dequeue_index = dequeue(&link);                        ///< Dequeue a packet from the link queue
                link_dequeue_count -= 1; ///< Decrease the dequeue count
            }

            if (packet == PACKET_LABEL_ACCEPT) ///< Further check if the packet is ACCEPT
            {
                enqueue(&link, ALPHA, tenant, &drop_tenant); ///< Enqueue the ACCEPT packet into the link queue
                if (drop_tenant != UNFOUND)                                                 ///< Check if a packet was dropped
                    if (drop_tenant != tenant)                                              ///< If the dropped packet is not from the current tenant
                        (flows+drop_tenant)->link_dropped_count += 1; ///< Increment the drop count for the other tenant
                    else
                        (flows+tenant)->link_dropped_count += 1; ///< Increment the drop count for the current tenant
                continue;
            }

            if(packet == PACKET_LABEL_GCRA_LABELED){
                enqueue(&link, BETA, tenant, &drop_tenant); ///< Enqueue the GCRA labeled packet into the link queue

                if (drop_tenant != UNFOUND)                                             ///< Check if a packet was dropped
                    (flows+drop_tenant)->link_dropped_count += 1; ///< Increment the drop count for the tenant whose packet was dropped

                continue;
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
            link_dequeue_count -= 1;                           ///< Decrease the dequeue count
        }

    }


    count_loss(flows, config.tenant_number);
#ifdef PRINT_PACKET_COUNT
    print_packet_count(flows, config.tenant_number);
#endif

#ifdef PRINT_EXECUTION_TIME
    execute_clock = clock() - execute_clock;                      ///< Calculate the elapsed execution time.
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC; ///< Convert clock ticks to seconds.
    printf("Execute time : %f s\n", time_taken);                  ///< Print the execution time in seconds.
#endif

    // Free allocated memory for the CSV configuration
    free_csv_configuration(csv_config);
    return EXIT_SUCCESS;
}