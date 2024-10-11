#include <math.h>

#ifdef SIMPLE_V2_925_GCRA
    #ifndef TOKEN_BUCKET_H
        #include "../Enthalten/token_bucket.h"
    #endif 
#endif

#if (defined SIMPLE_V1_00_918) || (defined SIMPLE_V2_925_GCRA)
    #define GUARDED_TRAFFIC 0   /**< @brief Tag for guarded traffic type. */
    #define MIDDLE_INTERVAL 1   /**< @brief Tag for combined middle interval traffic type. */
    #define DROPPED         2   /**< @brief Tag for dropped traffic type. */
    #define GAUSSIAN_MODE   3

#ifdef SIMPLE_V1_00_918
    #define DATA_STORED_PATH   "../Datei/module_test/module_test.csv"
#endif

#ifdef SIMPLE_V2_925_GCRA
    #define DATA_STORED_PATH   "../Datei/module_test/module_test.csv"
#endif

#endif

#define _TENANT_H /**< @brief Macro to enable tenant-related functionalities. */

#ifdef _TENANT_H

// About the traffic generation
/**
 * @brief Generates a single normally distributed random number.
 * 
 * This function uses the Box-Muller transform to generate a normally distributed
 * random number with a specified mean and standard deviation.
 * 
 * @param mean The mean of the normal distribution.
 * @param standard_deviation The standard deviation of the normal distribution.
 * 
 * @return A normally distributed random number.
 */
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

/**
 * @brief Returns a one-dimensional array following a normal distribution.
 * 
 * This function generates an array of normally distributed values with specified
 * mean and standard deviation. The length of the array is determined by the 
 * `time_interval` parameter.
 * 
 * @param time_interval The length of the array.
 * @param mean The mean of the normal distribution.
 * @param standard_deviation The standard deviation of the normal distribution.
 * 
 * @return A pointer to the array of normally distributed values.
 */
double* generateNormalDistribution(long time_interval, double mean, double standard_deviation) {
    double* traffic = (double*)malloc(sizeof(double)*time_interval);
    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
        *(traffic+time_stamp) = generateGaussian(mean, standard_deviation);
    return traffic;
}

double* generateRegularTraffic(long time_interval, double mean, double standard_deviation){
    double* traffic = (double*)malloc(sizeof(double)*time_interval);
    double sum = 0;

    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++){
        *(traffic+time_stamp) = ((double)rand()/RAND_MAX)*2-1;
        sum+=*(traffic+time_stamp);
        // printf("mean = %f\n", *(traffic+time_stamp));
    }

    double _mean = sum/time_interval;
    
    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
        *(traffic+time_stamp) -= _mean;

    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
        *(traffic+time_stamp) = *(traffic+time_stamp)*standard_deviation + mean;
    
    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++){
        if(*(traffic+time_stamp) >=160){
            // printf("%3d : "INFORM_DOUBLE_FORMAT"\n", time_stamp, *(traffic+time_stamp));
            *(traffic+time_stamp) =159.0;
        }
            
        // printf("%f\n", *(traffic+time_stamp));
    }
    return (double*)traffic;
}


/**
 * @struct Tenant
 * @brief Represents a network tenant.
 * 
 * This structure represents a network tenant with associated traffic data and
 * connection information.
 * 
 * @var Tenant::identifier
 * Tenant identifier.
 * @var Tenant::traffic
 * Array of traffic values.
 * @var Tenant::connected_node
 * Node to which the tenant is connected.
 * 
 * @ifdef SIMPLE_V1_00_918
 * @var Tenant::tag
 * Tag indicating the type of traffic.
 * @endif
 */
struct Tenant{
    unsigned int identifier;

    double* traffic;

#ifdef _NODE_H
    struct Node connected_node;
#endif

#if (defined SIMPLE_V1_00_918) || (defined SIMPLE_V2_925_GCRA)
    int tag;
#endif

#ifdef SIMPLE_V2_925_GCRA
    struct Token_Bucket bucket;
#endif
};

// Create a tenant
#if (defined SIMPLE_V1_00_918) || (defined SIMPLE_V2_925_GCRA)
    /**
     * @brief Creates a new tenant with specified parameters.
     * 
     * This function initializes a tenant with a unique identifier, generates traffic 
     * data based on the provided parameters, and assigns a tag indicating the type of 
     * traffic. The tenant is also associated with a specified node.
     * 
     * @param tenant_number Pointer to the current tenant number, which will be incremented.
     * @param time_interval The length of the traffic array.
     * @param mean The mean of the normal distribution for traffic generation.
     * @param standard_deviation The standard deviation of the normal distribution for traffic generation.
     * @param tag The tag indicating the type of traffic.
     * @param connected_node The node to which the tenant is connected.
     * 
     * @return A `Tenant` structure initialized with the specified parameters.
     */
    struct Tenant createATenant(unsigned int* tenant_number, long time_interval, double mean, double standard_deviation, int tag, struct Node connected_node){
        struct Tenant tenant;
        
        tenant.identifier = *tenant_number;
        *tenant_number+=1;
        tenant.traffic = generateNormalDistribution(time_interval, mean, standard_deviation);

        tenant.tag = tag;
        switch (tag)
        {
            case GUARDED_TRAFFIC:
                for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
                    if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
                        tenant.traffic[time_stamp] = mean + standard_deviation*SCALE_GUARDED_TRAFFIC;
                break;
            
            case DROPPED:
                for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
                    if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
                        tenant.traffic[time_stamp] = mean + standard_deviation*SCALE_DROPPED_TRAFFIC;
                break;

            case MIDDLE_INTERVAL:
                for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
                    if(COMPLY_MODULE_OR_NORMALDISTRIBUTION>0)
                        if(GENERATE_BINARY>0)
                            tenant.traffic[time_stamp] = mean + standard_deviation*SCALE_MIDDLE_INTERVAL;
                        else tenant.traffic[time_stamp] = mean - standard_deviation*SCALE_MIDDLE_INTERVAL;
                break;

            default:
                break;
        }
        tenant.connected_node = connected_node;

#ifdef SIMPLE_V2_925_GCRA
        tenant.bucket = createATokenBucket(BUCKET_DEPTH, LEAKAGE_RATE);
#endif
        return tenant;
    }
#endif

#ifdef SIMPLE_V1_00_918
// Show the traffic
/**
 * @brief Prints the traffic data for a tenant at a specific timestamp.
 * 
 * This function prints the traffic data for a given tenant at a specific timestamp, 
 * including the type of traffic handling (guarded, dropped, or kept). It also updates 
 * the total link traffic based on the tenant's traffic data.
 * 
 * @param tenant The tenant whose traffic data is to be printed.
 * @param destination The destination node for the traffic.
 * @param time_stamp The timestamp of the traffic data.
 * @param link_traffic Pointer to the total link traffic, which will be updated.
 * @param mean The mean of the normal distribution for traffic data.
 * @param standard_deviation The standard deviation of the normal distribution for traffic data.
 */
void printTrafficATimestamp(struct Tenant tenant, struct Node destination, int time_stamp, double* link_traffic, double mean, double standard_deviation){
    printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" in node["INFORM_IDENTIFIER_FORMAT"] to node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].", tenant.identifier, tenant.traffic[time_stamp], tenant.connected_node.identifier, destination.identifier, time_stamp);
    if(tenant.traffic[time_stamp]<=mean-standard_deviation){
        *link_traffic += tenant.traffic[time_stamp];
        printf("[GUARDED]\n");
    }else if (tenant.traffic[time_stamp]>mean+standard_deviation)
        printf("[DROPPED]\n");
    else printf("[MIDDLE INTERVAL]\n");
}
#endif

#ifdef SIMPLE_V2_925_GCRA
// Show the traffic
/**
 * @brief Prints the traffic data for a tenant at a specific timestamp.
 * 
 * This function prints the traffic data for a given tenant at a specific timestamp, 
 * including the type of traffic handling (guarded, dropped, or kept). It also updates 
 * the total link traffic based on the tenant's traffic data.
 * 
 * @param tenant The tenant whose traffic data is to be printed.
 * @param destination The destination node for the traffic.
 * @param time_stamp The timestamp of the traffic data.
 * @param link_traffic Pointer to the total link traffic, which will be updated.
 * @param mean The mean of the normal distribution for traffic data.
 * @param standard_deviation The standard deviation of the normal distribution for traffic data.
 */
void printTrafficATimestamp(struct Tenant* tenant, struct Node destination, int time_stamp, double* link_traffic, double mean, double standard_deviation){
#ifdef RECORD
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "a+");
    fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", tenant->traffic[time_stamp]);
    fclose(file_pointer);
#endif

#ifdef _DEBUG_PRINTTRAFFICATIMESTAMP
    printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" in node["INFORM_IDENTIFIER_FORMAT"] to node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].", tenant->identifier, tenant->traffic[time_stamp], tenant->connected_node.identifier, destination.identifier, time_stamp);
#endif
    if(tenant->traffic[time_stamp]<=mean-standard_deviation){
        *link_traffic += tenant->traffic[time_stamp];
        updateTokenBucketATimestamp(&(tenant->bucket), tenant->traffic[time_stamp]);
#ifdef RECORD
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "a+");
    fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", tenant->traffic[time_stamp]);
    fclose(file_pointer);
#endif
#ifdef _DEBUG_PRINTTRAFFICATIMESTAMP
        printf("[GUARDED]\n");
#endif
    }else if (tenant->traffic[time_stamp]>mean+standard_deviation){
        updateTokenBucketATimestamp(&(tenant->bucket), tenant->traffic[time_stamp]);
#ifdef _DEBUG_PRINTTRAFFICATIMESTAMP
        printf("[DROPPED]\n");
#endif
#ifdef RECORD
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "a+");
    fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", 0);
    fclose(file_pointer);
#endif
    }
    else{
        double leakage_traffic = updateTokenBucketATimestamp(&(tenant->bucket), tenant->traffic[time_stamp]);
#ifdef OVER_CAPACITY_DROP
        if(leakage_traffic == 0)
#elif (defined OVER_CAPACITY_LIMIT)
        if(leakage_traffic <tenant->traffic[time_stamp])
#endif
        {
#ifdef _DEBUG_PRINTTRAFFICATIMESTAMP
            printf("[DROPPED GCRA]\n");
#endif
#ifdef RECORD
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "a+");
    fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", 0);
    fclose(file_pointer);
#endif
        }
        else{
            *link_traffic += leakage_traffic;
#ifdef _DEBUG_PRINTTRAFFICATIMESTAMP
            printf("[MIDDLE INTERVAL GCRA]\n");
#endif
#ifdef RECORD
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "a+");
    fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", leakage_traffic);
    fclose(file_pointer);
#endif
        }
    }

}
#endif

#ifdef SIMPLE_V1_00_918
#endif

// Record
#ifdef SIMPLE_V1_00_918
/**
 * @brief Records traffic data for all tenants over an entire time interval.
 * 
 * This function calculates the total link traffic for all tenants at each timestamp 
 * within the specified time interval. It writes the traffic data to a CSV file, 
 * including individual tenant traffic, total link traffic, and the traffic ratio 
 * to link capacity.
 * 
 * @param tenants Pointer to an array of Tenant structures.
 * @param tenant_number The number of tenants.
 * @param time_interval The total number of timestamps to record.
 * @param link_capacity The maximum capacity of the link.
 * @param mean The mean of the normal distribution used for traffic generation.
 * @param standard_deviation The standard deviation of the normal distribution used for traffic generation.
 */
void recordTrafficEntireInterval(struct Tenant* tenants, unsigned int tenant_number, long time_interval, double link_capacity, double mean, double standard_deviation){
    system("rm "DATA_STORED_PATH"\n");
    FILE *file_pointer;
    file_pointer = fopen(DATA_STORED_PATH, "w+");

    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++){
        double link_traffic = 0.0;
        for(int index = 0;index<tenant_number;index++){
            if(tenants[index].traffic[time_stamp]<=mean-standard_deviation)
                link_traffic += tenants[index].traffic[time_stamp];
            
            if(tenants[index].traffic[time_stamp]<=mean+standard_deviation && tenants[index].traffic[time_stamp]>mean-standard_deviation)
                link_traffic += tenants[index].traffic[time_stamp];
                
            if(index==tenant_number-1)
                fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", "INFORM_TRAFFIC_FORMAT", "INFORM_TRAFFIC_FORMAT"\n", tenants[index].traffic[time_stamp], link_traffic, link_traffic/link_capacity);
            else fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ",tenants[index].traffic[time_stamp]);
        }
    }

    fclose(file_pointer);
}
#endif

#endif