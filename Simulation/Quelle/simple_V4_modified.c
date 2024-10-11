
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V4.ini"
#define PYTHON_IMAGE_PATH "./simple_V4.py"  /** @brief Path to the Python script for generating images or other visualization outputs. **/
#define DATA_STORED_PATH "../Datei/simple_V4/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V4/traffic.csv"
#define IMPORTANT_DATA_STORED_PATH "../Datei/simple_V4/important_data.csv"

#define PRINT
#define END_PRINT

#define _CODE_CAPACITY 0
#define _CODE_TENANT_NUMBER 1
#define _CODE_ERROR 2
#define _CODE_MEAN 3
#define _CODE_STANDARD_DEVIATION 4
#define _CODE_BUCKET_DEPTH 5
#define _CODE_LEAKAGE_RATE 6

// gcc ./simple_V4_modified.c inih/ini.c -o ../Ausführung/simple_V4_modified -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V4_modified 0 260

int main(int argc, char *argv[]){
    double capacity; /** @brief The total capacity for the system (e.g., bandwidth or resource limit). **/
    int tenant_number;  /** @brief The number of tenants in the system. **/
    long time_interval; /** @brief The time interval for token bucket processing (in milliseconds). **/
    double error;

    int gaussian = 0;
    double mean;    /** @brief The mean value for traffic generation. **/
    double standard_deviation;  /** @brief The standard deviation used for traffic generation. **/
    double bucket_depth;    /** @brief The maximum depth of the token bucket. **/
    double leakage_rate;    /** @brief The rate at which tokens leak from the bucket (tokens per time interval). **/

    configuration config;
    char command[MAX_COMMAND_LENGTH];

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
      printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
      return EXIT_FAILURE;
    }

    double min_transfer_rate = 0.0;
    double first_alpha = 0.0;
    double last_alpha = 0.0;

    switch(atoi(argv[1])){
        case _CODE_CAPACITY:
            config.capacity = atof(argv[2]);
        break;
        case _CODE_TENANT_NUMBER:
            config.tenant_number = atof(argv[2]);
        break;
        case _CODE_ERROR:
            config.error = atof(argv[2]);
        break;
        case _CODE_MEAN:
            config.mean = atof(argv[2]);
        break;
        case _CODE_STANDARD_DEVIATION:
            config.standard_deviation = atof(argv[2]);
        break;
        case _CODE_BUCKET_DEPTH:
            config.bucket_depth = atof(argv[2]);
        break;
        case _CODE_LEAKAGE_RATE:
            config.leakage_rate = atof(argv[2]);
        break;
        default:
            config.capacity = 360;
            config.tenant_number = 3;
            config.error = 0.001;
            config.mean = 120;
            config.standard_deviation = 40;
            config.bucket_depth = 200;
            config.leakage_rate = 120;
        break;
    }


    capacity = config.capacity;           /** @brief 1 : Link capacity **/
    tenant_number = config.tenant_number;      /** @brief 2 : Number of tenants **/
    time_interval = config.time_interval;     /** @brief 3 : Simulation time (with time units) **/
    error = config.error;  
    gaussian = config.gaussian;
    mean = config.mean;               /** @brief 5 : Mean of the traffic **/
    standard_deviation = config.standard_deviation; /** @brief 6 : Standard derivation(Covariance) of the traffic **/
    bucket_depth = config.bucket_depth;       /** @brief 7 : GCRA bucket size(depth) **/
    leakage_rate = config.leakage_rate;       /** @brief 8 : GCRA leakage rate **/
    

    modify_ini_file(CONFIGURATION_PATH, &config);

    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index = 0; index < tenant_number; index++){
        *(tenants + index) = (struct Tenant){index, NULL};
        double* traffic;

        if(gaussian == 0)
            traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
        else traffic = generateNormalDistribution(time_interval, mean, standard_deviation);
        // printf("%d\n", config.time_interval);
        (*(tenants + index)).traffic = traffic;
    }

    config.bucket_depth = atof(argv[2]);
    // printf("%f\n", bucket_depth);

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
    /**
     * @brief Closes the traffic file after recording is complete.
     */
    fclose(traffic_file);


    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 8 %d", tenant_number);
    system(command);
    min_transfer_rate = read_double_from_file("../Datei/transrate.txt");
#ifdef PRINT
    printf("min_rate = "INFORM_DOUBLE_FORMAT"\n", min_transfer_rate);
#endif
#endif


    

    sprintf(command, "python3 ./CVXPY.py 3 "CONFIGURATION_PATH);
    system(command);
    first_alpha = read_double_from_file("../Datei/objective.txt");
    capacity = (read_double_from_file("../Datei/objective.txt")) * capacity;

#ifdef PRINT
    printf("alpha        = %f\n", read_double_from_file("../Datei/objective.txt"));
    printf("New capacity = " INFORM_DOUBLE_FORMAT "\n", capacity);
#endif

    sprintf(command, "python3 ./CVXPY.py 4 "CONFIGURATION_PATH" %f", capacity);
    system(command);
    last_alpha = read_double_from_file("../Datei/objective.txt");

#ifdef PRINT
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));
#endif

    FILE* data_file;
    data_file = fopen(IMPORTANT_DATA_STORED_PATH, "a+");
        fprintf(data_file, INFORM_DOUBLE_FORMAT", "INFORM_DOUBLE_FORMAT", "INFORM_DOUBLE_FORMAT", "INFORM_DOUBLE_FORMAT"\n", atof(argv[2]), min_transfer_rate, first_alpha, last_alpha);
    fclose(data_file);

#ifdef END_PRINT
    printf("min_rate = "INFORM_DOUBLE_FORMAT"\n", min_transfer_rate);
    printf("alpha1   = "INFORM_DOUBLE_FORMAT"\n", first_alpha);
    printf("alpha2   = "INFORM_DOUBLE_FORMAT"\n", last_alpha);
#endif
    return EXIT_SUCCESS;
}