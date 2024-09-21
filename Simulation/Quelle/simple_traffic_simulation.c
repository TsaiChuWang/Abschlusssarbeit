/**
 * @file simple_traffic_simulation.c
 * @brief Test module for traffic simulation and link utilization.
 * 
 * This file contains code for generating normally distributed traffic,
 * simulating link utilization, and outputting the results.
 * 
 * Compilation:
 * @code
 * gcc ./simple_traffic_simulation.c -o ../Ausführung/simple_traffic_simulation -lm
 * @endcode
 * 
 * Execution:
 * @code
 * ../Ausführung/simple_traffic_simulation
 * @endcode
 */

#define SINPLE_V1_00_918  /** @brief Version identifier, **/ 

#include "../Enthalten/include.h"
#include "../Enthalten/configuration.h"
#include "../Enthalten/node.h"
#include "../Enthalten/link.h"
#include "../Enthalten/tenant.h"


int main(int argc, char *argv[]){
    // Initialize random number generator
    srand(time(NULL));

    double mean = TEST_CONFIGURATION_LINK_CAPACITY/TEST_CONFIGURATION_TENANT_NUMBER;
    double standard_deviation = mean/TEST_CONFIGURATION_TENANT_NUMBER;

    unsigned int node_number = 0;
    struct Node source = createANode(&node_number), destination = createANode(&node_number); 

    struct Link link = {0, TEST_CONFIGURATION_LINK_CAPACITY, source, destination};

    unsigned int tenant_number =0;
    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*TEST_CONFIGURATION_TENANT_NUMBER);
    *(tenants) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, GUARDED_TRAFFIC, source);
    *(tenants+1) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, MIDDLE_INTERVAL, source);
    *(tenants+2) = createATenant(&tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation, DROPPED, source);

    for(int time_stamp = 0;time_stamp<TIME_INTERVAL;time_stamp++){
        double link_traffic = 0;
        printf("mean = "INFORM_TRAFFIC_FORMAT", standard deviration = "INFORM_TRAFFIC_FORMAT"\n", mean, standard_deviation);

        printTrafficATimestamp(*(tenants), destination, time_stamp, &link_traffic, mean, standard_deviation);
        printTrafficATimestamp(*(tenants+1), destination, time_stamp, &link_traffic, mean, standard_deviation);
        printTrafficATimestamp(*(tenants+2), destination, time_stamp, &link_traffic, mean, standard_deviation);

        printf("traffic on link : "INFORM_TRAFFIC_FORMAT", utilization : "INFORM_TRAFFIC_FORMAT"\n", link_traffic, link_traffic/TEST_CONFIGURATION_LINK_CAPACITY);
        printf("\n");
    }

    recordTrafficEntireInterval(tenants, tenant_number, TEST_CONFIGURATION_TIME_INTERVAL, TEST_CONFIGURATION_LINK_CAPACITY, mean, standard_deviation);
    return  SUCCESS;
}