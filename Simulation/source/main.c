
#define CLEAN_DATA_PATH

#define PRINT_EXECUTION_TIME 
// #define PRINT_CAPACITY
// #define PRINT_REGULAR_AND_NAUGHTY
// #define PRINT_FIRST_INIT_GCRA
// #define PRINT_GCRA_UPDATE
// #define PRINT_EACH_TIMESTAMP
// #define PRINT_EACH_GRID_PACKET
#define PRINT_PACKET_COUNTS
#define PRINT_PACKET_LABEL

#define RECORD_REGULAR_AND_NAUGHTY_TAU
#define RECORD_REGULAR_AND_NAUGHTY_ALL
#define RECORD_AVERAGE_LOSS


#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/packets_count.h"
#include "../include/GCRA.h"

// #include "../include/link_capacity_queue.h"

// #include "../include/link_capacity_queue.h"


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
    printf("capacity : %f bps\n", capacity * config.unit);
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
    init_packets_count(&count, tenant_number, obtain_grids_number(config.packet_size));
    
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


    /** @brief Stores the total number of grids processed. */
    int grid_counts = 0;

    /**
     * @brief Main loop for simulating the traffic over time.
     * @details This loop runs until the timestamp exceeds the total simulation time. 
     *          The timestamp is incremented by the step size at each iteration.
     */
    while (timestamp <= (TIME_TYPE)(config.simulation_time * ONE_SECOND_IN_NS))
    {
        timestamp += (TIME_TYPE)(generator.step_size);

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
        }
//         int dequeue_count = 0;
//         while (link.dequeue_timestamp <= (double)timestamp)
//         {
//             link.dequeue_timestamp += link.dequeue_interval;
//             dequeue_count += 1;
//         }
//         // printf("dequeue_count = %d\n", dequeue_count);

//         int packet_time_count = 0;

//         for (int tenant = 0; tenant < tenant_number; tenant++)
//         {
//             if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
//             {
//                 packet_time_count += 1;
//                 *(count.count + tenant) += 1;
//             }
//             else
//                 continue;


//             if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
//             {
//                 long rate = (long)(timestamp - (gcras + tenant)->last_time) * (((double)(config.mean) * config.unit) / ONE_SECOND_IN_NS);

//                 long x = (long)((gcras + tenant)->x - rate);

// #ifdef PRINT_MAX_X
//                 if (x > max_x)
//                     max_x = x;
// #endif

// #ifdef PRINT_GCRA
//                 if (tenant == 99)
//                 {
//                     if (timestamp - (gcras + tenant)->last_time > 4069)
//                         printf("[%2d]lst = %9lf, time = %-7f, inter = %7lf, rate = %6ld x= %6ld\n", tenant, (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x);
//                     else
//                         printf("[%2d]lst = %9lf, time = %-7f, inter = \x1B[1;31m%6lf\x1B[0m, rate = %6ld x= %6ld drop = %d\n", tenant, (gcras + tenant)->last_time, timestamp, timestamp - (gcras + tenant)->last_time, rate, x, t);
//                     printf("x = %ld, tau = %ld %d\n", x, (gcras + tenant)->tau, x > (gcras + tenant)->tau);
//                 }
// #endif
//                 if (x > (gcras + tenant)->tau)
//                 {
//                     if (tenant == 99)
//                         t += 1;
//                     GCRAdrop += 1;
//                     *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
//                     label.labels[tenant][*(packets + tenant)] += 1;
//                 }
//                 else
//                 {
//                     (gcras + tenant)->x = MAX((long)0, x) + (gcras + tenant)->l;
//                     (gcras + tenant)->last_time = timestamp;
//                     *(packets + tenant) = PACKET_LABEL_ACCEPT;
//                 }
//             }

//             // if (*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
//             // {
//             //     // GCRAdrop += 1;
//             //             }

//             if (*(packets + tenant) == PACKET_LABEL_ACCEPT || *(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
//             {
//                 if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
//                 {
//                     *(packets + tenant) = enqueueLink(&link, tenant, ALPHA, &drop_tenant);
//                     if (dequeue_count > 0)
//                     {
//                         dequeueLink(&link);
//                         dequeue_count -= 1;
//                     }
//                 }

//                 if (*(packets + tenant) == PACKET_LABEL_GCRA_DROPPED)
//                 {
//                     *(packets + tenant) = enqueueLink(&link, tenant, BETA, &drop_tenant);
//                 }
//             }

//             // printf("teant = %d drop_tenant = %d\n", tenant, drop_tenant);
//             if (drop_tenant != -1 && drop_tenant < tenant_number)
//                 label.labels[drop_tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;

//             // printf("%d %d\n", *(packets + tenant), *(packets + tenant) == PACKET_LABEL_OVER_CAPACITY_DROPPED);
//             // RECORD:
//             if (*(packets + tenant) == PACKET_LABEL_OVER_CAPACITY_DROPPED)
//             {
//                 label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] += 1;
//             }
//             else
//                 label.labels[tenant][*(packets + tenant)] += 1;
//         }

//         //         // printf("front = %2d rear = %2d front = %2d rear = %2d\n", link.alpha_front,
//         //         //        link.alpha_rear,
//         //         //        link.beta_front,
//         //         //        link.beta_rear);

//         //         int t = -1;
//         while (dequeue_count > 0)
//         {
//             dequeueLink(&link);
//             dequeue_count -= 1;
//         }

//         // printf("packet_time_count = %d\n", packet_time_count);
//         // printf("front = %2d rear = %2d front = %2d rear = %2d\n", link.alpha_front,
//         //        link.alpha_rear,
//         //        link.beta_front,
//         //        link.beta_rear);
    }



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
    if(config.traffic_mode == TRAFFIC_MODE_NAUGHTY || config.traffic_mode >= TRAFFIC_MODE_BURSTY_ALL){
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