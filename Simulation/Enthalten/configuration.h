#define TEST
#define DEBUG

#define _CONFIGURATION_H
#ifdef TEST

#define TIME_INTERVAL 100
#define TIME_UNIT 0

#define DISCRETEEVENT

#define KBPS 1

#endif

#define RECORD
// #define GENERATE_GAUSSIAN_TRAFFIC

#if (defined SIMPLE_V1_00_918) || (defined SIMPLE_V2_925_GCRA)
    #define TEST_CONFIGURATION_LINK_CAPACITY    360 /** @brief Link capacity for test configuration **/
    #define TEST_CONFIGURATION_TIME_INTERVAL    100 /** @brief Time interval for the test configuration **/
    #define TEST_CONFIGURATION_TENANT_NUMBER    3   /** @brief Number of tenants in the test configuration **/

    #define PROBABILITY_COMPLY_MODULE           30  /** @brief Probability of complying module (in percentage). **/
    /** @def COMPLY_MODULE_OR_NORMALDISTRIBUTION
     *  @brief Decide between module or Gaussian distribution based on probability.
     *  @details 1 for module, 0 for Gaussian.
     */
    #define COMPLY_MODULE_OR_NORMALDISTRIBUTION (int)((rand()%100)+1)>PROBABILITY_COMPLY_MODULE?0:1 

    #define PROBABILITY_DROPPED                 10  /** Probability of traffic being dropped (in percentage). **/
    /** @def DECIDE_DROPPED_OR_NOT
     *  @brief Decide if the traffic is dropped based on probability.
     *  @details 1 for dropped, 0 for keep.
     */
    #define DECIDE_DROPPED_OR_NOT               (int)((rand()%100)+1)>PROBABILITY_DROPPED?0:1

    #define SCALE_GUARDED_TRAFFIC               (-1)*(1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05)) /** @brief Scale factor for guarded traffic. **/
    #define SCALE_MIDDLE_INTERVAL               (0.5 + ((double)rand()/RAND_MAX)*(0.95-0.5))        /** Scale factor for middle interval traffic. **/
    #define SCALE_DROPPED_TRAFFIC               (1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05))      /** Scale factor for dropped traffic. **/

#ifdef SIMPLE_V2_925_GCRA
    #define BUCKET_DEPTH 200
    #define LEAKAGE_RATE 120

    // #define _DEBUG_UPDATETOKENBUCKETATIMESTAMP
    #define _DEBUG_PRINTTRAFFICATIMESTAMP

    #define GENERATE_GIF

    #define OVER_CAPACITY_LIMIT
    // #define OVER_CAPACITY_DROP
#endif
    
    // #define _DEBUG_PRINTTRAFFICATIMESTAMP
#endif

#ifdef _CONFIGURATION_H
#include <string.h>

    typedef struct{
        double capacity; /** @brief The total capacity for the system (e.g., bandwidth or resource limit). **/
        int tenant_number;  /** @brief The number of tenants in the system. **/
        long time_interval; /** @brief The time interval for token bucket processing (in milliseconds). **/
        double error;

        int gaussian;
        double mean;    /** @brief The mean value for traffic generation. **/
        double standard_deviation;  /** @brief The standard deviation used for traffic generation. **/
        
        double bucket_depth;    /** @brief The maximum depth of the token bucket. **/
        double leakage_rate;    /** @brief The rate at which tokens leak from the bucket (tokens per time interval). **/
    }configuration;

    static int handler(void* config, const char* section, const char* name, const char* value){
        configuration* pconfig = (configuration*)config;
    
#define MATCH(sect, nam) strcmp(section, sect) == 0 && strcmp(name, nam) == 0
     
        if(MATCH("simulation", "capacity"))
            pconfig->capacity = atof(value);
        else if(MATCH("simulation", "tenant_number"))
            pconfig->tenant_number = atoi(value);
        else if(MATCH("simulation", "time_interval"))
            pconfig->time_interval = (long)atoi(value);
        else if(MATCH("simulation", "error"))
            pconfig->error = atof(value);
        else if(MATCH("traffic", "gaussian"))
            pconfig->gaussian = atoi(value);
        else if(MATCH("traffic", "mean"))
            pconfig->mean = atof(value);
        else if(MATCH("traffic", "standard_deviation"))
            pconfig->standard_deviation = atof(value);
        else if(MATCH("GCRA", "bucket_depth")) 
            pconfig->bucket_depth = atof(value);
        else if(MATCH("GCRA", "leakage_rate")) 
            pconfig->leakage_rate = atof(value);
        else return 0;  
        
        return SUCCESS;
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

    void modify_ini_file(const char* filename, configuration* config) {
        FILE* file = fopen(filename, "w");
        if (!file) {
            perror("Unable to open file for writing");
            return;
        }

        fprintf(file, "[simulation]\n");
        fprintf(file, "capacity = %f\n", config->capacity);
        fprintf(file, "tenant_number = %d\n", config->tenant_number);
        fprintf(file, "time_interval = %d\n", config->time_interval);
        fprintf(file, "error = %f\n", config->error);

        fprintf(file, "[traffic]\n");
        fprintf(file, "gaussian = %d\n", config->gaussian);
        fprintf(file, "mean = %f\n", config->mean);
        fprintf(file, "standard_deviation = %f\n", config->standard_deviation);

        fprintf(file, "[GCRA]\n");
        fprintf(file, "bucket_depth = %f\n", config->bucket_depth);
        fprintf(file, "leakage_rate = %f\n", config->leakage_rate);
        fclose(file);
    }
#endif