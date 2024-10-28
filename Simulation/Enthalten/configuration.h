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

    #define INITIAL_CONFIGURATION_TENANT_NUMBER 14
    #define INITIAL_CONFIGURATION_TIME_INTERVAL 100
    #define INITIAL_CONFIGURATION_ERROR 0.001

    #define INITIAL_CONFIGURATION_GAUSSIAN 0
    #define INITIAL_CONFIGURATION_MEAN 120.0
    #define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40.0

    #define INITIAL_CONFIGURATION_NAUGHTY_MEAN 150
    #define INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION 10
    #define INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER 7
    
    #define INITIAL_CONFIGURATION_BUCKET_DEPTH 285.5160353
    #define INITIAL_CONFIGURATION_LEAKAGE_RATE 120.0
    
    /**
     * @def TRAFFIC_MODE_REGULAR
     * @brief Traffic mode for regular (non-Gaussian) traffic generation.
     *
     * This mode represents standard traffic generation without Gaussian distribution.
     */
    #define TRAFFIC_MODE_REGULAR 0

    /**
     * @def TRAFFIC_MODE_GAUSSIAN
     * @brief Traffic mode for Gaussian distribution-based traffic generation.
     *
     * This mode enables traffic generation based on a Gaussian (normal) distribution, 
     * using parameters such as mean and standard deviation.
     */
    #define TRAFFIC_MODE_GAUSSIAN 1

    /**
     * @def TRAFFIC_MODE_NAUGHTY_TENANT_INCLUDE
     * @brief Traffic mode that includes "naughty" tenants in traffic generation.
     *
     * This mode accounts for tenants that generate irregular or bursty traffic, often 
     * characterized by a separate Gaussian distribution with its own mean and standard deviation.
     */
    #define TRAFFIC_MODE_NAUGHTY_TENANT_INCLUDE 2

    /**
     * @struct configuration
     * @brief Configuration settings for the token bucket algorithm and traffic generation.
     * 
     * This structure holds parameters that define the system's capacity, tenant details, 
     * and GCRA algorithm settings, along with traffic generation settings using Gaussian distribution.
     */

    typedef struct {
        double  capacity;       /**< @brief The total capacity for the system (e.g., bandwidth or resource limit). */
        int     tenant_number;  /**< @brief The number of tenants in the system. */
        long    time_interval;  /**< @brief The time interval for token bucket processing (in milliseconds). */
        double  error;          /**< @brief The allowed error margin or variance for traffic control. */

        int     gaussian;           /**< @brief A flag indicating whether Gaussian traffic generation is enabled (1 = enabled, 0 = disabled). */
        double  mean;               /**< @brief The mean value for traffic generation (used in Gaussian distribution). */
        double  standard_deviation; /**< @brief The standard deviation for traffic generation (used in Gaussian distribution). */
        
        double  naughty_mean;               /**< @brief The mean value for generating traffic for "naughty" tenants (those that may overuse resources). */
        double  naughty_standard_deviation; /**< @brief The standard deviation for traffic generation of "naughty" tenants. */
        int     naughty_tenant_number;      /**< @brief The number of "naughty" tenants (those that generate irregular traffic). */

        double  bucket_depth;   /**< @brief The maximum depth of the token bucket (i.e., the maximum tokens the bucket can hold). */
        double  leakage_rate;   /**< @brief The rate at which tokens leak from the bucket (tokens per time interval). */

        long tau;
        long l;
    } configuration;

    /**
     * @brief INI file handler function for loading configuration settings.
     * 
     * This function processes INI file sections and key-value pairs, updating the fields 
     * of the `configuration` structure based on the provided values.
     * 
     * @param config A pointer to the `configuration` structure to update.
     * @param section The section of the INI file currently being processed.
     * @param name The key name in the INI file section.
     * @param value The corresponding value for the key in the INI file.
     * 
     * @return SUCCESS if the key-value pair is successfully matched and processed; 
     *         0 if the key-value pair is not matched.
     */
    static int handler(void* config, const char* section, const char* name, const char* value){
        configuration* pconfig = (configuration*)config;  /**< Cast config to configuration type. */
    
    #define MATCH(sect, nam) (strcmp(section, sect) == 0 && strcmp(name, nam) == 0) /**< Macro to match section and name. */
     
        if (MATCH("simulation", "capacity")) 
            pconfig->capacity = atof(value);            /**< Set capacity from string value. */
        else if (MATCH("simulation", "tenant_number")) 
            pconfig->tenant_number = atoi(value);       /**< Set tenant_number from string value. */
        else if (MATCH("simulation", "time_interval")) 
            pconfig->time_interval = (long)atoi(value); /**< Set time_interval from string value. */
        else if (MATCH("simulation", "error")) 
            pconfig->error = atof(value);               /**< Set error from string value. */
    
        else if (MATCH("traffic", "gaussian")) 
            pconfig->gaussian = atoi(value);                    /**< Set Gaussian traffic mode from string value. */
        else if (MATCH("traffic", "mean")) 
            pconfig->mean = atof(value);                        /**< Set mean for traffic generation from string value. */
        else if (MATCH("traffic", "standard_deviation")) 
            pconfig->standard_deviation = atof(value);          /**< Set standard deviation for traffic generation from string value. */
        else if (MATCH("traffic", "naughty_mean")) 
            pconfig->naughty_mean = atof(value);                /**< Set mean for "naughty" tenant traffic from string value. */
        else if (MATCH("traffic", "naughty_standard_deviation")) 
            pconfig->naughty_standard_deviation = atof(value);  /**< Set standard deviation for "naughty" tenant traffic from string value. */
        else if (MATCH("traffic", "naughty_tenant_number")) 
            pconfig->naughty_tenant_number = atoi(value);       /**< Set number of "naughty" tenants from string value. */

        else if (MATCH("GCRA", "bucket_depth")) 
            pconfig->bucket_depth = atof(value);    /**< Set bucket depth for GCRA (Generic Cell Rate Algorithm) from string value. */
        else if (MATCH("GCRA", "leakage_rate")) 
            pconfig->leakage_rate = atof(value);    /**< Set leakage rate for GCRA from string value. */
        else if (MATCH("GCRA", "tau")) 
            pconfig->tau = strtol(value, NULL, 10);    /**< Set bucket depth for GCRA (Generic Cell Rate Algorithm) from string value. */
        else if (MATCH("GCRA", "l")) 
            pconfig->l = strtol(value, NULL, 10);    /**< Set leakage rate for GCRA from string value. */
        else return EXIT_FAILURE;                              /**< Return 0 if no match is found for the section and name. */
        
        return SUCCESS; /**< Return SUCCESS if the key-value pair was processed successfully. */
    }

    /**
     * @brief Reads a double value from a specified file.
     * 
     * This function opens a file in read mode and attempts to read a double value from it.
     * If the file cannot be opened or if the reading fails, it returns an error value (-1.0).
     *
     * @param filename The name of the file to read the double value from.
     * 
     * @return The double value read from the file, or -1.0 if an error occurs (such as failure 
     *         to open the file or read the value correctly).
     */
    double read_double_from_file(const char *filename) {
        FILE *file;      /**< File pointer for accessing the file. */
        double value;    /**< Variable to store the double value read from the file. */

        // Open the file in read mode
        file = fopen(filename, "r");
        if (file == NULL) {
            perror("Error opening file");  /**< Print error if the file cannot be opened. */
            return FAILED;  /**< Return error value if file can't be opened. */
        }

        // Read the double value from the file
        if (fscanf(file, "%lf", &value) != 1) {
            perror("Error reading double from file");  /**< Print error if reading fails. */
            fclose(file);  /**< Close the file before returning. */
            return FAILED;   /**< Return error value if reading fails. */
        }

        // Close the file
        fclose(file);  /**< Close the file after reading the value. */

        return value;  /**< Return the successfully read double value. */
    }

    /**
     * @brief Modifies an INI configuration file by writing updated values from the `configuration` structure.
     * 
     * This function opens an INI file in write mode and writes the updated values from the `configuration` structure 
     * to the file. The INI file is divided into sections such as `[simulation]`, `[traffic]`, and `[GCRA]`, and each 
     * section's fields are updated with the corresponding values from the structure.
     *
     * @param filename The name of the INI file to modify.
     * @param config A pointer to the `configuration` structure containing the updated values to be written.
     * 
     * @note The function will overwrite the contents of the INI file.
     */
    void modify_ini_file(const char* filename, configuration* config) {
        // Open the file in write mode
        FILE* file = fopen(filename, "w");
        if (!file) {
            perror("Unable to open file for writing");  /**< Print error message if file cannot be opened. */
            return;
        }

        // Write the [simulation] section
        fprintf(file, "[simulation]\n");
        fprintf(file, "capacity = %f\n", config->capacity);  /**< Write the capacity to the INI file. */
        fprintf(file, "tenant_number = %d\n", config->tenant_number);  /**< Write the tenant number to the INI file. */
        fprintf(file, "time_interval = %ld\n", config->time_interval);  /**< Write the time interval to the INI file. */
        fprintf(file, "error = %f\n", config->error);  /**< Write the error margin to the INI file. */

        // Write the [traffic] section
        fprintf(file, "[traffic]\n");
        fprintf(file, "gaussian = %d\n", config->gaussian);  /**< Write whether Gaussian traffic is enabled. */
        fprintf(file, "mean = %f\n", config->mean);  /**< Write the mean value for traffic generation. */
        fprintf(file, "standard_deviation = %f\n", config->standard_deviation);  /**< Write the standard deviation for traffic generation. */
        fprintf(file, "naughty_mean = %f\n", config->naughty_mean);  /**< Write the mean for "naughty" tenant traffic. */
        fprintf(file, "naughty_standard_deviation = %f\n", config->naughty_standard_deviation);  /**< Write the standard deviation for "naughty" tenant traffic. */
        fprintf(file, "naughty_tenant_number = %d\n", config->naughty_tenant_number);  /**< Write the number of "naughty" tenants. */

        // Write the [GCRA] section
        fprintf(file, "[GCRA]\n");
        fprintf(file, "bucket_depth = %f\n", config->bucket_depth);  /**< Write the bucket depth for GCRA. */
        fprintf(file, "leakage_rate = %f\n", config->leakage_rate);  /**< Write the leakage rate for GCRA. */
        fprintf(file, "tau = %ld\n", config->tau);  /**< Write the bucket depth for GCRA. */
        fprintf(file, "l = %ld\n", config->l);  /**< Write the leakage rate for GCRA. */

        // Close the file
        fclose(file);  /**< Close the file after writing. */
    }
#endif