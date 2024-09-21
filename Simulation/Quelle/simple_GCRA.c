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

#define SIMPLE_V2_925_GCRA  /** @brief Version identifier, **/ 

#include "../Enthalten/include.h"
#include "../Enthalten/configuration.h"
#include "../Enthalten/node.h"
#include "../Enthalten/link.h"
#include "../Enthalten/token_bucket.h"
#include "../Enthalten/tenant.h"

int main(int argc, char *argv[]){
    // Initialize random number generator
    srand(time(NULL));

    double mean = TEST_CONFIGURATION_LINK_CAPACITY/TEST_CONFIGURATION_TENANT_NUMBER;
    double standard_deviation = mean/TEST_CONFIGURATION_TENANT_NUMBER;

    unsigned int node_number = 0;
    struct Node source = createANode(&node_number), destination = createANode(&node_number); 

    struct Link link = {0, TEST_CONFIGURATION_LINK_CAPACITY, source, destination};

    struct Token_Bucket* buckets = createMultipleTokenBucket_SameParamter(BUCKET_DEPTH, LEAKAGE_RATE, TEST_CONFIGURATION_TENANT_NUMBER);

    unsigned int tenant_number =0;
    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*TEST_CONFIGURATION_TENANT_NUMBER);
    *(tenants) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, GUARDED_TRAFFIC, source);
    *(tenants+1) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, MIDDLE_INTERVAL, source); // MIDDLE_INTERVAL
    *(tenants+2) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, DROPPED, source);

    printf(INFORM_TRAFFIC_FORMAT"\n", (double)((*tenants).bucket.bucket_depth));
#if (defined SIMPLE_V2_925_GCRA) && (defined RECORD)
    system("rm "DATA_STORED_PATH"\n");
    system("rm "TOKEN_BUCKET_DATA_STORED_PATH"\n");
#endif
    for(int time_stamp = 0;time_stamp<TIME_INTERVAL;time_stamp++){
        double link_traffic = 0;
        // printf("mean = "INFORM_TRAFFIC_FORMAT", standard deviration = "INFORM_TRAFFIC_FORMAT"\n", mean, standard_deviation);

        printTrafficATimestamp((tenants), destination, time_stamp, &link_traffic, mean, standard_deviation);
        printTrafficATimestamp((tenants+1), destination, time_stamp, &link_traffic, mean, standard_deviation);
        printTrafficATimestamp((tenants+2), destination, time_stamp, &link_traffic, mean, standard_deviation);

        printf("traffic on link : "INFORM_TRAFFIC_FORMAT", utilization : "INFORM_TRAFFIC_FORMAT"\n", link_traffic, link_traffic/TEST_CONFIGURATION_LINK_CAPACITY);

#ifdef RECORD
        FILE *file_pointer;
        file_pointer = fopen(DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", "INFORM_TRAFFIC_FORMAT"\n", link_traffic, link_traffic/TEST_CONFIGURATION_LINK_CAPACITY);
        fclose(file_pointer);

        file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, "\n");
        fclose(file_pointer);
#endif
        printf("\n");
    }

    // recordTrafficEntireInterval(tenants, tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, TEST_CONFIGURATION_LINK_CAPACITY, mean, standard_deviation);
    return  SUCCESS;
}