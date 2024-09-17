#include "../Enthalten/include.h"
#include "../Enthalten/configuration.h"
// #include "../Enthalten/event.h"
// #include "../Enthalten/tenant.h"
// #include "../Enthalten/node.h"
#include <time.h>
#include <math.h>

// gcc ./module_test.c -o ../Ausführung/module_test -lm
// ../Ausführung/module_test

#define SINPLE_V1_00_918  

#define TEST_CONFIGURATION_LINK_CAPACITY    360
#define TEST_CONFIGURATION_TIME_INTERVAL    100
#define TEST_CONFIGURATION_TENANT_NUMBER    3

#define PROBABILITY_COMPLY_MODULE   30  // in percentage(%)
#define COMPLY_MODULE_OR_NORMALDISTRIBUTION (int)((rand()%100)+1)>PROBABILITY_COMPLY_MODULE?0:1 // 1 : Module, 0: Gaussian
#define PROBABILITY_DROPPED  10  // in percentage(%)
#define DECIDE_DROPPED_OR_NOT   (int)((rand()%100)+1)>PROBABILITY_DROPPED?0:1 // 1 : dropped, 0: keep

#define GENERATE_BINARY (int)(rand()%2)

#define SCALE_GUARDED_TRAFFIC               (-1)*(1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05))
#define SCALE_PROBABLE_DROPPED_TRAFFIC      (-1)*(0.5 + ((double)rand()/RAND_MAX)*(0.95-0.5))
#define SCALE_UNKNOWN_PUNISHMENT_TRAFFIC    (0.5 + ((double)rand()/RAND_MAX)*(0.95-0.5))
#define SCALE_DROPPED_TRAFFIC               (1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05))


// Function to generate a single normally distributed random number
double generateGaussian(double mean, double standard_deviation) {
    double u1, u2, omega, multiplier;
    static double x1, x2;
    static int call = 0;

    if (call == 1) {
        call = !call;
        return (mean + standard_deviation * x2);
    }

    do {
        u1 = -1 + ((double)rand() / RAND_MAX) * 2;
        u2 = -1 + ((double)rand() / RAND_MAX) * 2;
        omega = u1 * u1 + u2 * u2;
    } while (omega >= 1 || omega == 0);

    multiplier = sqrt((-2 * log(omega)) / omega);
    x1 = u1 * multiplier;
    x2 = u2 * multiplier;

    call = !call;

    return (mean + standard_deviation * x1);
}

// Function to return a one-dimensional array following a normal distribution
double* generateNormalDistribution(long time_interval, double mean, double standard_deviation) {
    double* traffic = (double*)malloc(sizeof(double)*time_interval);
    for(int time_stamp = 0;time_stamp<TIME_INTERVAL;time_stamp++)
        *(traffic+time_stamp) = generateGaussian(mean, standard_deviation);
    return traffic;
}


#define COMPLY_MODULE_TRAFFIC 

struct Node{
    unsigned int identifier;
};

struct Link{
    unsigned int identifier;

    double capacity;
    struct Node head_node;
    struct Node tail_node;
};

struct Tenant{
    unsigned int identifier;

    double* traffic;
    struct Node connected_node;
};

int main(int argc, char *argv[]){
    // Initialize random number generator
    srand(time(NULL));

    double mean = TEST_CONFIGURATION_LINK_CAPACITY/TEST_CONFIGURATION_TENANT_NUMBER;
    double standard_deviation = mean/TEST_CONFIGURATION_TENANT_NUMBER;

    struct Node source = {0};
    struct Node destination = {1};

    struct Link link = {0, TEST_CONFIGURATION_LINK_CAPACITY, source, destination};

    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*TEST_CONFIGURATION_TENANT_NUMBER);

    double* traffic_0 = generateNormalDistribution(TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation);
    double* traffic_1 = generateNormalDistribution(TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation);
    double* traffic_2 = generateNormalDistribution(TEST_CONFIGURATION_TIME_INTERVAL, mean, standard_deviation);

    for(int time_stamp = 0;time_stamp<TIME_INTERVAL;time_stamp++){
        if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
            *(traffic_0+time_stamp) = mean + standard_deviation*SCALE_GUARDED_TRAFFIC;
        
        if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
            if(GENERATE_BINARY>0)   
                *(traffic_1+time_stamp) = mean + standard_deviation*SCALE_UNKNOWN_PUNISHMENT_TRAFFIC;   // 1 : UNKNOWN PUNISHMENT
            else *(traffic_1+time_stamp) = mean + standard_deviation*SCALE_PROBABLE_DROPPED_TRAFFIC ; // 0 : DISCARDED WITH PROBABILITY

        if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
            *(traffic_2+time_stamp) = mean + standard_deviation*SCALE_DROPPED_TRAFFIC;
    }
    struct Tenant tenant_0 = {0, traffic_0, source};
    struct Tenant tenant_1 = {1, traffic_1, source};
    struct Tenant tenant_2 = {2, traffic_2, source};

    *tenants = tenant_0;
    *(tenants+1) = tenant_1;
    *(tenants+2) = tenant_2;

    for(int time_stamp = 0;time_stamp<TIME_INTERVAL;time_stamp++){
        double link_traffic = 0;

        // printf("mean = "INFORM_TRAFFIC_FORMAT", standard deviration = "INFORM_TRAFFIC_FORMAT"\n", mean, standard_deviation);

        printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" kbps in node["INFORM_IDENTIFIER_FORMAT"] to node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].", tenants[0].identifier, tenants[0].traffic[time_stamp], tenants[0].connected_node.identifier, destination.identifier, time_stamp);
        if(tenants[0].traffic[time_stamp]<=mean-standard_deviation){
            link_traffic += tenants[0].traffic[time_stamp];
            printf("[GUARDED]\n");
        }else if (tenants[0].traffic[time_stamp]>mean+standard_deviation)
            printf("[DROPPED]\n");
        else if(DECIDE_DROPPED_OR_NOT>0)
            printf("[DROPPED_PROBABLE]\n");
        else{
            link_traffic += tenants[0].traffic[time_stamp];
            printf("[KEEPED_PROBABLE]\n");
        }

        printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" kbps in node["INFORM_IDENTIFIER_FORMAT"] to node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].", tenants[1].identifier, tenants[1].traffic[time_stamp], tenants[1].connected_node.identifier, destination.identifier, time_stamp);
        if(tenants[1].traffic[time_stamp]<=mean-standard_deviation){
            link_traffic += tenants[1].traffic[time_stamp];
            printf("[GUARDED]\n");
        }else if (tenants[1].traffic[time_stamp]>mean+standard_deviation)
            printf("[DROPPED]\n");
        else if(DECIDE_DROPPED_OR_NOT>0)
            printf("[DROPPED_PROBABLE]\n");
        else{
            link_traffic += tenants[1].traffic[time_stamp];
            printf("[KEEPED_PROBABLE]\n");
        }

        printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" kbps in node["INFORM_IDENTIFIER_FORMAT"] to node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].", tenants[2].identifier, tenants[2].traffic[time_stamp], tenants[2].connected_node.identifier, destination.identifier, time_stamp);
        if(tenants[2].traffic[time_stamp]<=mean-standard_deviation){
            link_traffic += tenants[2].traffic[time_stamp];
            printf("[GUARDED]\n");
        }else if (tenants[2].traffic[time_stamp]>mean+standard_deviation)
            printf("[DROPPED]\n");
        else if(DECIDE_DROPPED_OR_NOT>0)
            printf("[DROPPED_PROBABLE]\n");
        else{
            link_traffic += tenants[2].traffic[time_stamp];
            printf("[KEEPED_PROBABLE]\n");
        }

        printf("traffic on link : "INFORM_TRAFFIC_FORMAT", utilization : "INFORM_TRAFFIC_FORMAT"\n", link_traffic, link_traffic/TEST_CONFIGURATION_LINK_CAPACITY);

        printf("\n");
    }
    return  SUCCESS;
}