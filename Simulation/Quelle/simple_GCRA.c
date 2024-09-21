/**
 * @file simple_GCRA.c
 * @brief Test module for traffic simulation and link utilization.
 *
 * This file contains code for generating normally distributed traffic,
 * simulating link utilization, and outputting the results.
 *
 * Compilation:
 * @code
 * gcc ./simple_GCRA.c -o ../Ausführung/simple_GCRA -lm
 * @endcode
 *
 * Execution:
 * @code
 * ../Ausführung/simple_GCRA
 * @endcode
 */

#define SIMPLE_V2_925_GCRA /** @brief Version identifier, **/

#include "../Enthalten/configuration.h"
#include "../Enthalten/include.h"
#include "../Enthalten/node.h"
#include "../Enthalten/link.h"
#include "../Enthalten/token_bucket.h"
#include "../Enthalten/tenant.h"

/**
 * @brief Main function for traffic simulation and link utilization.
 * 
 * This function initializes nodes, tenants, and links, generates traffic
 * for each tenant based on normal distribution, and simulates traffic flow
 * over a link. It also prints link utilization and stores traffic data if 
 * RECORD is defined.
 * 
 * @param argc Argument count from the command line.
 * @param argv Command line arguments:
 *        - argv[1]: Bucket depth (optional, default from config).
 *        - argv[2]: Leakage rate (optional, default from config).
 * @return Returns 0 on success.
 */
int main(int argc, char *argv[]) {
    // Initialize random number generator
    srand(time(NULL));

    double bucket_depth, leakage_rate;
    
    /**
     * @brief Set bucket depth and leakage rate based on command-line input or default values.
     */
    if (argc < 2) {
        bucket_depth = BUCKET_DEPTH;
        leakage_rate = LEAKAGE_RATE;
    } else {
        char *end_pointer;
        bucket_depth = strtod(argv[1], &end_pointer); /**< Convert argv[1] to double for bucket depth. */
        leakage_rate = strtod(argv[2], &end_pointer); /**< Convert argv[2] to double for leakage rate. */
    }

    double mean = TEST_CONFIGURATION_LINK_CAPACITY / TEST_CONFIGURATION_TENANT_NUMBER;
    double standard_deviation = mean / TEST_CONFIGURATION_TENANT_NUMBER;

    unsigned int node_number = 0;

    /**
     * @brief Create source and destination nodes for the link.
     */
    struct Node source = createANode(&node_number),
                destination = createANode(&node_number);

    /**
     * @brief Initialize the link between source and destination nodes.
     */
    struct Link link = {0, TEST_CONFIGURATION_LINK_CAPACITY, source, destination};

    /**
     * @brief Create multiple token buckets for traffic control, using the same parameters.
     */
    struct Token_Bucket *buckets = createMultipleTokenBucket_SameParamter(
        bucket_depth, leakage_rate, TEST_CONFIGURATION_TENANT_NUMBER);

    unsigned int tenant_number = 0;

    /**
     * @brief Allocate memory for tenants and create each tenant with different traffic patterns.
     */
    struct Tenant *tenants = (struct Tenant *)malloc(
        sizeof(struct Tenant) * TEST_CONFIGURATION_TENANT_NUMBER);
    
    *(tenants) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL,
                               mean, standard_deviation, GUARDED_TRAFFIC, source);
    *(tenants + 1) = createATenant(
        &tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean,
        standard_deviation, MIDDLE_INTERVAL, source); /**< Tenant with MIDDLE_INTERVAL traffic pattern. */
    *(tenants + 2) =
        createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean,
                      standard_deviation, DROPPED, source); /**< Tenant with DROPPED traffic pattern. */

    printf(INFORM_TRAFFIC_FORMAT "\n", (double)((*tenants).bucket.bucket_depth));

    #if (defined SIMPLE_V2_925_GCRA) && (defined RECORD)
    /**
     * @brief Remove existing traffic data files if RECORD is defined.
     */
    system("rm " DATA_STORED_PATH "\n");
    system("rm " TOKEN_BUCKET_DATA_STORED_PATH "\n");
    #endif

    /**
     * @brief Main loop for traffic simulation over time intervals.
     */
    for (int time_stamp = 0; time_stamp < TIME_INTERVAL; time_stamp++) {
        double link_traffic = 0;

        /**
         * @brief Generate and print traffic for each tenant at each timestamp.
         */
        printTrafficATimestamp((tenants), destination, time_stamp, &link_traffic,
                               mean, standard_deviation);
        printTrafficATimestamp((tenants + 1), destination, time_stamp,
                               &link_traffic, mean, standard_deviation);
        printTrafficATimestamp((tenants + 2), destination, time_stamp,
                               &link_traffic, mean, standard_deviation);

        /**
         * @brief Print link traffic and utilization at each timestamp.
         */
        printf("traffic on link : " INFORM_TRAFFIC_FORMAT
               ", utilization : " INFORM_TRAFFIC_FORMAT "\n",
               link_traffic, link_traffic / TEST_CONFIGURATION_LINK_CAPACITY);

        #ifdef RECORD
        /**
         * @brief Record link traffic and utilization data if RECORD is defined.
         */
        FILE *file_pointer;
        file_pointer = fopen(DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT ", " INFORM_TRAFFIC_FORMAT "\n",
                link_traffic, link_traffic / TEST_CONFIGURATION_LINK_CAPACITY);
        fclose(file_pointer);

        file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, "\n");
        fclose(file_pointer);
        #endif

        printf("\n");
    }

    return SUCCESS;
}
