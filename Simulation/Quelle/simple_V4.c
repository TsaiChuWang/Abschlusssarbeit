
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V4.ini"
#define PYTHON_IMAGE_PATH "./simple_V4.py"  /** @brief Path to the Python script for generating images or other visualization outputs. **/
#define DATA_STORED_PATH "../Datei/simple_V4/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V4/traffic.csv"

// gcc ./simple_V4.c inih/ini.c -o ../Ausführung/simple_V4 -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V4
// ../Ausführung/simple_V4 c
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

    if(argc<2){ // Argument parsing and default assignments
        capacity = 360;
        tenant_number = 3;
        time_interval = 100;
        mean = 120;
        standard_deviation = 40;
        bucket_depth = 40;
        leakage_rate = 120;
        error = 0.01;
    }else if(strcmp(argv[1], "a")==0){
        if(argc>8){
            capacity = atof(argv[1]);           /** @brief 1 : Link capacity **/
            tenant_number = atoi(argv[2]);      /** @brief 2 : Number of tenants **/
            time_interval = atoi(argv[3]);      /** @brief 3 : Simulation time (with time units) **/
            gaussian = atoi(argv[4]);
            mean = atof(argv[5]);               /** @brief 5 : Mean of the traffic **/
            standard_deviation = atof(argv[6]); /** @brief 6 : Standard derivation(Covariance) of the traffic **/
            bucket_depth = atof(argv[7]);       /** @brief 7 : GCRA bucket size(depth) **/
            leakage_rate = atof(argv[8]);       /** @brief 8 : GCRA leakage rate **/
            error = 0.01;
        }else{
            printf("argument 1 : Link capacit\n");
            printf("argument 2 : Number of tenants\n");
            printf("argument 3 : Simulation time (with time units\n");
            printf("argument 4 : 0 for generating regular traffic, 1 for generating gaussian traffic\n");
            printf("argument 5 : Mean of the traffic\n");
            printf("argument 6 : Standard derivation(Covariance) of the traffic\n");
            printf("argument 7 : GCRA bucket size(depth\n");
            printf("argument 8 : GCRA leakage rate\n");
            perror("Missing argument!\n");
            return EXIT_FAILURE;
        }
    }else if(strcmp(argv[1], "c")==0){
        if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
            printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
            return EXIT_FAILURE;
        }

        capacity = config.capacity;           /** @brief 1 : Link capacity **/
        tenant_number = config.tenant_number;      /** @brief 2 : Number of tenants **/
        time_interval = config.time_interval;     /** @brief 3 : Simulation time (with time units) **/
        gaussian = config.gaussian;
        mean = config.mean;               /** @brief 5 : Mean of the traffic **/
        standard_deviation = config.standard_deviation; /** @brief 6 : Standard derivation(Covariance) of the traffic **/
        bucket_depth = config.bucket_depth;       /** @brief 7 : GCRA bucket size(depth) **/
        leakage_rate = config.leakage_rate;       /** @brief 8 : GCRA leakage rate **/
        error = config.error;  
    }

    
    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index = 0; index < tenant_number; index++){
        *(tenants + index) = (struct Tenant){index, NULL};
        double* traffic;

        if(gaussian == 0)
            traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
        else traffic = generateNormalDistribution(time_interval, mean, standard_deviation);
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
            sprintf(command, "python3 " PYTHON_IMAGE_PATH " 1 %d", index);
        else 
            sprintf(command, "python3 " PYTHON_IMAGE_PATH " 4 %d " INFORM_DOUBLE_FORMAT " " INFORM_DOUBLE_FORMAT, index, mean, standard_deviation);
        
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
    /**
     * @brief Closes the traffic file after recording is complete.
     */
    fclose(traffic_file);

    /**
     * @brief Executes a Python script to generate initial images or visualizations.
     * 
     * This first call runs the Python script with the mode 0, which is likely used for initializing the visualization process.
     */
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 0");
    system(command);

    /**
     * @brief Executes the Python script for each tenant to generate tenant-specific images.
     * 
     * Depending on whether arguments are provided, the Python script is called with different modes and arguments.
     * - Mode 2: If no arguments are provided, the tenant index is passed.
     * - Mode 5: If arguments are provided, the tenant index, mean, and standard deviation are passed.
     */
    for(int index = 0; index < tenant_number; index++) {
        if(argc < 2)
            sprintf(command, "python3 " PYTHON_IMAGE_PATH " 2 %d", index);
        else
            sprintf(command, "python3 " PYTHON_IMAGE_PATH " 5 %d " INFORM_DOUBLE_FORMAT " " INFORM_DOUBLE_FORMAT, index, mean, standard_deviation);
        system(command);
    }

    /** @brief Executes the Python script again in mode 0 to refresh or finalize the visualizations after all tenants are processed. **/
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 0");
    system(command);

    /**
     * @brief Executes the Python script with mode 3 to generate images or output based on the total system capacity.
     * 
     * The system capacity is passed as a parameter to the Python script.
     */
    if(argc < 2)
        sprintf(command, "python3 " PYTHON_IMAGE_PATH " 3 %f", capacity);
    else sprintf(command, "python3 " PYTHON_IMAGE_PATH " 6 %f %d", capacity, tenant_number);
    system(command);

#endif
    sprintf(command, "python3 ./CVXPY.py 0 "CONFIGURATION_PATH);
    system(command);
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 0");
    system(command);
    sprintf(command, "python3 ./CVXPY.py 1 "CONFIGURATION_PATH);
    system(command);
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 0");
    system(command);

    printf("alpha        = %f\n", read_double_from_file("../Datei/objective.txt"));
    capacity = (read_double_from_file("../Datei/objective.txt")-error) * capacity;
    printf("New capacity = " INFORM_DOUBLE_FORMAT "\n", capacity);

    if(argc < 2)
        sprintf(command, "python3 " PYTHON_IMAGE_PATH " 3 %f", capacity);
    else sprintf(command, "python3 " PYTHON_IMAGE_PATH " 7 %f %d", capacity, tenant_number);
    system(command);

    sprintf(command, "python3 ./CVXPY.py 2 "CONFIGURATION_PATH" %f", capacity);
    system(command);
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));

    return EXIT_SUCCESS;
}