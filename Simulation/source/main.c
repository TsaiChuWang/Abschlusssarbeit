
// #define CLEAN_DATA_PATH

// #define PRINT_GRID_COUNT
// #define PRINT_EXECUTION_TIME 
#define PRINT_CAPACITY
// #define PRINT_REGULAR_AND_NAUGHTY
// #define PRINT_FIRST_INIT_GCRA
// #define PRINT_GCRA_UPDATE
// #define PRINT_EACH_TIMESTAMP
// #define PRINT_EACH_GRID_PACKET
#define PRINT_PACKET_COUNTS
#define PRINT_PACKET_LABEL
// #define PRINT_DEQUEUE_COUNT

// #define RECORD_REGULAR_AND_NAUGHTY_TAU
// #define RECORD_REGULAR_AND_NAUGHTY_ALL
#define RECORD_AVERAGE_LOSS


#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/packets_count.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"


#define CONFIGURATION_PATH "../configuration/main.ini"

#define NAME "main"

// gcc ./main.c inih/ini.c -o ../execution/main -lm
// ../execution/main [traffic_generation_code]

#define GENERATE_TRAFFIC 1

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
     // show_configuration(config); // Uncomment if you need to debug configuration settings.
 
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
    double capacity = obtain_capacity();

#ifdef PRINT_CAPACITY
    /**  @brief Prints the calculated network capacity. */
    printf("capacity : %f bps\n", capacity );
#endif

    /** @brief Initializes the random seed for random number generation.*/
    srand48(time(NULL));

    /** @brief Initializes the traffic generator based on the configuration. */
    traffic_generator generator = initializeTrafficGenerator(config);
    // showTrafficGenerator(generator);  ///< Uncomment to display generator details.

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

    // queue for the exceed upper bound traffic (reservation)

    /** @brief Array of GCRA (Generic Cell Rate Algorithm) structures for traffic shaping. */
    GCRA *gcras = initializeGCRAs(tenant_number, config.tau, config.packet_size);
#ifdef PRINT_FIRST_INIT_GCRA
    /** @brief Prints the details of the first initialized GCRA instance. */
    show_GCRA(*(gcras));
#endif


    link_priority_queue link;
    initlink_priority_queue(&link, config.link_queue_buffer, config, capacity); // Max buffer size

    
    /** @brief Stores the total number of grids processed. */
    int grid_counts = 0;
    int drop_tenant = UNFOUND;
    

    /**
     * @brief Main loop for simulating the traffic over time.
     * @details This loop runs until the timestamp exceeds the total simulation time. 
     *          The timestamp is incremented by the step size at each iteration.
     */
    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {
        
        timestamp += (TIME_TYPE)(generator.step_size);

        // Count the dequeue times
        int dequeue_count = 0;

        while (link.dequeue_timestamp <= (double)timestamp)
        {
            link.dequeue_timestamp += link.dequeue_interval;
            dequeue_count += 1;
        }

#ifdef PRINT_DEQUEUE_COUNT
        printf("dequeue_count = %d\n", dequeue_count);
#endif

#ifdef PRINT_EACH_TIMESTAMP
        printf("timestamp : %-lf\n", timestamp);
#endif
        grid_counts++;

        // Packets generation
        int* packets = packet_generation_configuration(generator, config);
#ifdef PRINT_EACH_GRID_PACKET
        print_packets(packets, config.tenant_number);
#endif

        for (int tenant = 0; tenant < tenant_number; tenant++){
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                *(count.count + tenant) += 1;
            }else continue; // It is no need to operation if there is no packet

            // Upper bound exceeded traffic operation(reservation)

            // Packet through GCRA
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                *(packets + tenant) = gcra_update(timestamp, (gcras+tenant), config);
            }

            // Statistic th number of packet labeled as GCRA
            if(*(packets + tenant) == PACKET_LABEL_GCRA_LABELED)
                label.labels[tenant][PACKET_LABEL_GCRA_LABELED] += 1;

                
            // Link queue enqueue
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_LABELED)
            {
                if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                {
                    if (dequeue_count > 0)
                    {
                        int index = dequeue(&link);
                        if(index != UNFOUND)
                            label.labels[index][PACKET_LABEL_ACCEPT] += 1;
                        dequeue_count -= 1;
                    }
                    enqueue(&link, ALPHA, tenant, &drop_tenant);

                    if(drop_tenant != UNFOUND)
                        if(drop_tenant != tenant)
                            label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;
                        else label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;
                }

                if (*(packets + tenant) == PACKET_LABEL_GCRA_LABELED)
                {
                    enqueue(&link, BETA, tenant, &drop_tenant);
                    if(drop_tenant != UNFOUND)
                        label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;
                }
            }
        }

        while (dequeue_count > 0)
        {
            int index = dequeue(&link);
            if(index != UNFOUND)
                label.labels[index][PACKET_LABEL_ACCEPT] += 1;
            dequeue_count -= 1;
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
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
        print_regular_and_naughty(label, config.naughty_tenant_number);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_TAU
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL ){
        record_regular_and_naughty_tau(label, config);
    }
#endif

#ifdef RECORD_REGULAR_AND_NAUGHTY_ALL
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
        record_regular_and_naughty_all(label, config);
    }
#endif

#ifdef RECORD_AVERAGE_LOSS
    record_average_loss(label, config);
#endif

#ifdef PRINT_EXECUTION_TIME 
    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC;
    printf("Execute time : %f s\n", time_taken);
#endif

    return EXIT_SUCCESS;
}