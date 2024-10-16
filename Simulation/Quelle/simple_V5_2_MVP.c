/**
 * @file simple_V5_2_MVP.c
 * @brief Implementation of network traffic management using GCRA algorithm.
 */

#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h"  /**< Configuration header file for network settings */
#include "../Enthalten/tenant.h"         /**< Tenant data management for network resources */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"                  /**< INI file parser library */

/** 
 * @def CONFIGURATION_PATH
 * @brief Path to the INI configuration file.
 */
#define CONFIGURATION_PATH "../configuration/simple_V5.ini"

/** 
 * @def PYTHON_PATH
 * @brief Path to the Python script that may be invoked for additional processing.
 */
#define PYTHON_PATH "./simple_V5.py"

/** 
 * @def DATA_STORED_PATH
 * @brief Format string for the path where traffic data for each tenant is stored in CSV format.
 * 
 * The tenant ID will be formatted into the string using `traffic_%d.csv`.
 */
#define DATA_STORED_PATH "../Datei/simple_V5/traffic_%d.csv"

/** 
 * @def BUCKET_DATA_STORED_PATH
 * @brief Path where the overall traffic data for the token bucket algorithm is stored.
 */
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V5/traffic.csv"

/**
 * @brief Compile command for the program.
 *
 * This program can be compiled using the following command:
 * 
 * @code
 * gcc ./simple_V5_2_MVP.c inih/ini.c -o ../Ausführung/simple_V5_2_MVP -lgsl -lgslcblas -lm
 * @endcode
 * 
 * @details This command links the necessary libraries: 
 * - `gsl` and `gslcblas` for scientific computation (GNU Scientific Library).
 * - `libm` for mathematical functions.
 *
 * @example
 * After compilation, the executable can be run with the following command:
 * 
 * @code
 * ../Ausführung/simple_V5_2_MVP c
 * @endcode
 */

int main(int argc, char *argv[]){

    double capacity;    // optimizw with the model

    int tenant_number;  /** @brief The number of tenants in the system. **/
    long time_interval; /** @brief The time interval for token bucket processing (in milliseconds). **/
    double error;
    
    int gaussian = 2;
    double mean;    /** @brief The mean value for traffic generation. **/
    double standard_deviation;  /** @brief The standard deviation used for traffic generation. **/
    double naughty_mean;
    double naughty_standard_deviation;
    int naughty_tenant_number;

    double bucket_depth;    /** @brief The maximum depth of the token bucket. **/
    double leakage_rate;    /** @brief The rate at which tokens leak from the bucket (tokens per time interval). **/

    configuration config;
    char command[MAX_COMMAND_LENGTH];

    if(argc<2){ // Argument parsing and default assignments
        tenant_number = INITIAL_CONFIGURATION_TENANT_NUMBER;
        time_interval = INITIAL_CONFIGURATION_TIME_INTERVAL;
        error = INITIAL_CONFIGURATION_ERROR;

        gaussian = INITIAL_CONFIGURATION_GAUSSIAN;
        mean = 120;
        standard_deviation = 40;
        bucket_depth = 40;
        leakage_rate = 120;
        
        naughty_mean = 150;

         14
    #define  100
    #define  0.001

    #define  0
    #define INITIAL_CONFIGURATION_MEAN 120.0
    #define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40.0

    #define INITIAL_CONFIGURATION_NAUGHTY_MEAN 150
    #define INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION 10
    #define INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER 7
    
    #define INITIAL_CONFIGURATION_BUCKET_DEPTH 285.5160353
    #define INITIAL_CONFIGURATION_LEAKAGE_RATE 120.0
    }
    
    if(strcmp(argv[1], "c")==0){
        if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
            printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
            return EXIT_FAILURE;
        }

        tenant_number = config.tenant_number;      /** @brief 2 : Number of tenants **/
        time_interval = config.time_interval;     /** @brief 3 : Simulation time (with time units) **/
        gaussian = config.gaussian;
        mean = config.mean;               /** @brief 5 : Mean of the traffic **/
        standard_deviation = config.standard_deviation; /** @brief 6 : Standard derivation(Covariance) of the traffic **/
        bucket_depth = config.bucket_depth;       /** @brief 7 : GCRA bucket size(depth) **/
        leakage_rate = config.leakage_rate;       /** @brief 8 : GCRA leakage rate **/
        error = config.error;  
        naughty_mean = config.naughty_mean;
        naughty_standard_deviation = config.naughty_standard_deviation;
        naughty_tenant_number = config.naughty_tenant_number;
    }
    system("python3 " PYTHON_PATH " 0 "CONFIGURATION_PATH);
    capacity = read_double_from_file("../Datei/objective.txt");
    printf("Capacity = "INFORM_DOUBLE_FORMAT"\n", capacity);

    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index = 0; index < tenant_number; index++){
        *(tenants + index) = (struct Tenant){index, NULL};
        double* traffic;

        if(gaussian == 0)
            traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
        else if(gaussian == 1)
            traffic = generateNormalDistribution(time_interval, mean, standard_deviation);
        else if(gaussian == 2){
            if(index<naughty_tenant_number){
                traffic = generateNaughtyTraffic(time_interval, naughty_mean, naughty_standard_deviation);
                // printf("index = %d | %d\n", index, naughty_tenant_number);
            }
            else {
                traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
                // printf("index = %d / %d\n", index, naughty_tenant_number);
            }
        }
        
        // printf("%d\n", config.time_interval);
        (*(tenants + index)).traffic = traffic;

#ifdef RECORD
        /**  @brief Record traffic to a CSV file if the RECORD flag is defined. **/
        FILE* traffic_file;
        char path[MAX_PATH_LENGTH];
        sprintf(path, DATA_STORED_PATH, index);
        
        /** @brief Open the file and write the traffic data for the tenant. **/
        traffic_file = fopen(path, "w+");
        for(int time_stamp = 0; time_stamp < time_interval; time_stamp++)
            fprintf(traffic_file, INFORM_DOUBLE_FORMAT "\n", (*(tenants + index)).traffic[time_stamp]);
        
        fclose(traffic_file);

        /**
         * @brief Execute the Python script to generate visualizations.
         * 
         * The script is called with different arguments based on whether the program was invoked with arguments.
         */
        
        if(argc < 2)
            sprintf(command, "python3 " PYTHON_PATH " 1 %d", index);
        else 
            sprintf(command, "python3 " PYTHON_PATH " 2 %d " INFORM_DOUBLE_FORMAT " " INFORM_DOUBLE_FORMAT, index, mean, standard_deviation);
        
        system(command);
#endif
    }

#ifdef RECORD
    /**
     * @brief Opens the file to record bucket traffic data if the RECORD flag is defined.
     */
    FILE* traffic_file;
    traffic_file = fopen(BUCKET_DATA_STORED_PATH, "w+");
#endif
    struct Token_Bucket* buckets = createMultipleTokenBucket_SameParamter(bucket_depth, leakage_rate, tenant_number);

    for(int time_stamp = 0; time_stamp < time_interval; time_stamp++) {
        for(int index = 0; index < tenant_number; index++) {
            double traffic = (*(tenants + index)).traffic[time_stamp]; /**< Traffic for the current timestamp. */
            double leakage_traffic = 0.0; /**< Amount of traffic leaking from the bucket. */

            /**
             * @brief Handles traffic exceeding the mean plus the standard deviation.
             * 
             * If traffic is too high, tokens are added to the bucket, up to its maximum depth.
             */
            if(traffic > mean + standard_deviation) {
                (*(buckets + index)).bucket_capacity += (*(buckets + index)).leakage_rate;
                if((*(buckets + index)).bucket_capacity > (*(buckets + index)).bucket_depth)
                    (*(buckets + index)).bucket_capacity = (*(buckets + index)).bucket_depth;
                goto end_distinguish;
            }

            /**
             * @brief Handles the case where bucket capacity is insufficient to handle the traffic.
             * 
             * In this case, leakage traffic is limited by the remaining bucket capacity.
             */
            if((*(buckets + index)).bucket_capacity + (*(buckets + index)).leakage_rate - traffic < 0) {
                leakage_traffic = (*(buckets + index)).bucket_capacity + (*(buckets + index)).leakage_rate;
                (*(buckets + index)).bucket_capacity = 0;
                goto end_distinguish;
            }

            /** @brief Handles traffic that can be accommodated by the current bucket capacity. **/
            if((*(buckets + index)).bucket_capacity + (*(buckets + index)).leakage_rate - traffic > 0) {
                leakage_traffic = traffic;
                (*(buckets + index)).bucket_capacity = (*(buckets + index)).bucket_capacity + (*(buckets + index)).leakage_rate - traffic;
                if((*(buckets + index)).bucket_capacity > (*(buckets + index)).bucket_depth)
                    (*(buckets + index)).bucket_capacity = (*(buckets + index)).bucket_depth;
                goto end_distinguish;
            }

            /** @brief Handles traffic less than or equal to the mean minus the standard deviation.  **/
            if(traffic <= mean - standard_deviation) {
                leakage_traffic = traffic;
                (*(buckets + index)).bucket_capacity += (*(buckets + index)).bucket_capacity + (*(buckets + index)).leakage_rate - traffic;
                if((*(buckets + index)).bucket_capacity > (*(buckets + index)).bucket_depth)
                    (*(buckets + index)).bucket_capacity = (*(buckets + index)).bucket_depth;
                goto end_distinguish;
            }

end_distinguish:
#ifdef RECORD
            /** @brief Records traffic, leakage traffic, and bucket capacity at each timestamp. **/
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT ", ", (*(tenants + index)).traffic[time_stamp]);
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT ", ", leakage_traffic);
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT ", ", (*(buckets + index)).bucket_capacity);
#endif
#ifdef INFORM_BUCKET_SITUATION
            /** @brief Outputs the current bucket situation to the console. **/
            printf(INFORM_TRAFFIC_FORMAT " ", (*(tenants + index)).traffic[time_stamp]);
            printf(INFORM_TRAFFIC_FORMAT " ", leakage_traffic);
            printf(INFORM_TRAFFIC_FORMAT " ", (*(buckets + index)).bucket_capacity);
#endif
        }

#ifdef INFORM_BUCKET_SITUATION
        printf("\n"); /**< Outputs a new line for each timestamp when bucket information is printed. */
#endif
#ifdef RECORD
        fprintf(traffic_file, "\n"); /**< Records a new line for each timestamp in the traffic file. */
#endif
    }
#ifdef RECORD
    fclose(traffic_file);

    sprintf(command, "python3 " PYTHON_PATH " 3");
    system(command);

    for(int index = 0; index < tenant_number; index++) {
        if(argc < 2)
            sprintf(command, "python3 " PYTHON_PATH " 4 %d", index);
        else
            sprintf(command, "python3 " PYTHON_PATH " 5 %d " INFORM_DOUBLE_FORMAT " " INFORM_DOUBLE_FORMAT, index, mean, standard_deviation);
        system(command);
    }

    /** @brief Executes the Python script again in mode 0 to refresh or finalize the visualizations after all tenants are processed. **/
    sprintf(command, "python3 " PYTHON_PATH " 3");
    system(command);

    if(argc < 2)
        sprintf(command, "python3 " PYTHON_PATH " 6 %f", capacity);
    else sprintf(command, "python3 " PYTHON_PATH " 7 %f %d", capacity, tenant_number);
    system(command);

#endif

    return EXIT_SUCCESS;
}