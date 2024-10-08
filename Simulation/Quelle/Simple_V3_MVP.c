/**
 * @file Simple_V3_MVP.c
 * @brief Traffic management simulation using Token Bucket and CVXPY for optimization.
 *
 * This file simulates traffic using a token bucket algorithm and provides interaction
 * with a Python script for optimization using CVXPY. The generated traffic data is stored in CSV format.
 *
 * Compilation:
 * @code
 * gcc ./Simple_V3_MVP.c -o ../Ausführung/Simple_V3_MVP -lcurl -lgsl -lgslcblas -lm
 * @endcode
 *
 * Execution:
 * @code
 * ../Ausführung/Simple_V3_MVP
 *  ../Ausführung/Simple_V3_MVP 200 4 1000 50 12.5 100 50 100 50
 * @endcode
 */

#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */

/** 
 * @brief Path to store traffic data in CSV format.
 * 
 * The file path contains a placeholder `%d` for dynamically naming files.
 */
#define DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic_%d.csv"

/** 
 * @brief Path to store bucket-related traffic data in CSV format.
 * 
 * This path does not contain dynamic file naming.
 */
#define BUCKET_DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic.csv"

#define PYTHON_CVXPY_PATH "./CVXPY.py"  /** @brief Path to the Python script using CVXPY for optimization. **/
#define PYTHON_IMAGE_PATH "./Simgle_V3_MVP.py"  /** @brief Path to the Python script for generating images or other visualization outputs. **/

// #define INFORM_BUCKET_SITUATION /**< Uncomment to log bucket situation during execution */

/**
 * @brief Reads a double value from a specified file.
 *
 * This function opens a file in read mode, attempts to read a double-precision
 * floating-point value from the file, and returns the value. If the file
 * cannot be opened or the reading process fails, it returns an error value (-1.0).
 *
 * @param filename The path to the file from which to read the double value.
 * @return The double value read from the file, or -1.0 if an error occurs.
 */
double read_double_from_file(const char *filename);

int main(int argc, char *argv[]){

    double capacity; /** @brief The total capacity for the system (e.g., bandwidth or resource limit). **/
    int tenant_number;  /** @brief The number of tenants in the system. **/
    long time_interval; /** @brief The time interval for token bucket processing (in milliseconds). **/
    double mean;    /** @brief The mean value for traffic generation. **/
    double standard_deviation;  /** @brief The standard deviation used for traffic generation. **/
    double bucket_depth;    /** @brief The maximum depth of the token bucket. **/
    double leakage_rate;    /** @brief The rate at which tokens leak from the bucket (tokens per time interval). **/

    if(argc<2){ // Argument parsing and default assignments
        capacity = 360;
        tenant_number = 3;
        time_interval = 100;
        mean = 120;
        standard_deviation = 40;
        bucket_depth = 40;
        leakage_rate = 120;
    }else if(argc<8){
        printf("argument 1 : Link capacit\n");
        printf("argument 2 : Number of tenants\n");
        printf("argument 3 : Simulation time (with time units\n");
        printf("argument 4 : Mean of the traffic\n");
        printf("argument 5 : Standard derivation(Covariance) of the traffic\n");
        printf("argument 6 : GCRA bucket size(depth\n");
        printf("argument 7 : GCRA leakage rate\n");
        perror("Missing argument!\n");
        return EXIT_FAILURE;
    }else{
        capacity = atof(argv[1]);           /** @brief 1 : Link capacity **/
        tenant_number = atoi(argv[2]);      /** @brief 2 : Number of tenants **/
        time_interval = atoi(argv[3]);      /** @brief 3 : Simulation time (with time units) **/
        mean = atof(argv[4]);               /** @brief 4 : Mean of the traffic **/
        standard_deviation = atof(argv[5]); /** @brief 5 : Standard derivation(Covariance) of the traffic **/
        bucket_depth = atof(argv[6]);       /** @brief 6 : GCRA bucket size(depth) **/
        leakage_rate = atof(argv[7]);       /** @brief 7 : GCRA leakage rate **/
    }

    // Topologie erstellen (Create topology)
    /**
     * @brief Creates tenants and generates traffic for each tenant.
     * 
     * This block of code dynamically allocates memory for tenants, generates regular traffic for each,
     * and stores it in the corresponding tenant structure. If the `RECORD` flag is defined, the traffic 
     * is written to a CSV file and a Python script is called to generate images.
     */
    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index = 0; index < tenant_number; index++){
        // Initialize each tenant with an index and NULL traffic
        *(tenants + index) = (struct Tenant){index, NULL};

        /**
         * @brief Generate traffic using a regular traffic generator.
         * 
         * The traffic is generated based on the provided time interval, mean, and standard deviation.
         */
#ifndef GENERATE_GAUSSIAN_TRAFFIC 
        double* traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
#else
        double* traffic = generateNormalDistribution(time_interval, mean, standard_deviation);
#endif
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
        char command[MAX_COMMAND_LENGTH];
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

    /**
     * @brief Creates token buckets with the same parameters for all tenants.
     * 
     * This function creates multiple token buckets, each initialized with the specified bucket depth 
     * and leakage rate, and associates them with each tenant.
     */
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
    char command[MAX_COMMAND_LENGTH];
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
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 3 %f", capacity);
    system(command);

#endif
#ifndef RECORD
    /**
     * @brief Command buffer for executing Python scripts.
     * 
     * The `command` variable is used to store system commands that call Python scripts for
     * capacity calculations and visualization generation.
     */
    char command[MAX_COMMAND_LENGTH];
#endif

/**
 * @brief Executes the CVXPY Python script to calculate the system's capacity.
 * 
 * The script is called with the current `capacity` as an argument. The result is used to update the capacity value.
 */
    sprintf(command, "python3 ./CVXPY.py %f", capacity);
    system(command);

/**
 * @brief Reads and prints the scaling factor (alpha) from a file.
 * 
 * The scaling factor is stored in the file "../Datei/objective.txt", and it is used to adjust the capacity.
 */
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));

/**
 * @brief Updates the system capacity based on the alpha value.
 * 
 * The new capacity is calculated by multiplying the original capacity by the alpha factor.
 */
    capacity = read_double_from_file("../Datei/objective.txt") * capacity;
    printf("New capacity = " INFORM_DOUBLE_FORMAT "\n", capacity);

/**
 * @brief Executes the Python script to generate visualizations of the new capacity.
 * 
 * The script is called with the updated `capacity` to reflect changes in the visual output.
 */
    sprintf(command, "python3 " PYTHON_IMAGE_PATH " 3 %f", capacity);
    system(command);

/**
 * @brief Re-executes the CVXPY Python script to further refine the capacity.
 * 
 * The updated capacity is used as an argument, and the new alpha factor is printed.
 */
    sprintf(command, "python3 ./CVXPY.py %f", capacity);
    system(command);
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));

/** @brief Program exits successfully. **/
    return EXIT_SUCCESS;
}

/**
 * @brief Implementation of read_double_from_file function.
 *
 * @param filename The name of the file to read the double value from.
 * @return The double value from the file or -1.0 if an error occurs.
 */
double read_double_from_file(const char *filename) {
    FILE *file;
    double value;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1.0;  // Return an error value if file can't be opened
    }

    // Read the double value from the file
    if (fscanf(file, "%lf", &value) != 1) {
        perror("Error reading double from file");
        fclose(file);
        return -1.0;   // Return an error value if reading fails
    }

    // Close the file
    fclose(file);

    return value;
}