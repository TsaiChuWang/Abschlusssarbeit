
#include <string.h>
#include <float.h>
#include <limits.h>
#include <errno.h>

#define CONFIG_H

/**
 * @brief Enumeration defining the various traffic modes used for tenant network behavior.
 *
 * @details This enumeration provides different traffic distribution modes for network simulation:
 * - Uniform and Gaussian distributions for SLA-compliant traffic
 * - Non-compliant traffic patterns
 * - Density-based traffic
 * - Various bursty traffic scenarios using finite state machines
 */

/**
 * @def TRAFFIC_MODE_UNIFORM
 * @brief All tenants follow SLA with uniform traffic distribution
 */
#define TRAFFIC_MODE_UNIFORM 0

/**
 * @def TRAFFIC_MODE_GAUSSIAN
 * @brief All tenants follow SLA with Gaussian (normal) traffic distribution
 */
#define TRAFFIC_MODE_GAUSSIAN 1

/**
 * @def TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM
 * @brief All tenants violate SLA with uniform traffic distribution
 */
#define TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM 2

/**
 * @def TRAFFIC_MODE_NONCOMPLIANT_UNIFORM
 * @brief Mixed scenario: Some tenants violate SLA while others comply, using uniform distribution
 */
#define TRAFFIC_MODE_NONCOMPLIANT_UNIFORM 3

/**
 * @def TRAFFIC_MODE_DENSITY
 * @brief Generates traffic at maximum capacity (upper bound)
 */
#define TRAFFIC_MODE_DENSITY 4

/**
 * @def TRAFFIC_MODE_BURSTY_ALL
 * @brief All tenants generate bursty traffic using finite state machine model
 */
#define TRAFFIC_MODE_BURSTY_ALL 5

/**
 * @def TRAFFIC_MODE_BURSTY_COMPLIANT
 * @brief Only SLA-compliant tenants generate bursty traffic using finite state machine
 */
#define TRAFFIC_MODE_BURSTY_COMPLIANT 6

/**
 * @def TRAFFIC_MODE_BURSTY_NONCOMPLIANT
 * @brief Only SLA-non-compliant tenants generate bursty traffic using finite state machine
 */
#define TRAFFIC_MODE_BURSTY_NONCOMPLIANT 7

/**
 * @def TRAFFIC_MODE_DIFFERENT_R
 * @brief Both compliant and non-compliant tenants generate bursty traffic with different STATE_R parameters
 */
#define TRAFFIC_MODE_DIFFERENT_R 8

/**
 * @brief Enumeration defining different modes for handling compliant and non-compliant flows
 *
 * @details These modes specify various scenarios for flow distribution and timing:
 * - Temporal ordering of compliant flows
 * - Distribution patterns between compliant and non-compliant flows
 * - Extreme case scenarios with isolated flows
 */

/**
 * @def NONCOMPLIANT_MODE_BEFORE
 * @brief Positions compliant flows before other flows in the sequence
 * @details Ensures that compliant traffic flows are processed first in the timeline
 */
#define NONCOMPLIANT_MODE_BEFORE 0

/**
 * @def NONCOMPLIANT_MODE_AFTER
 * @brief Positions compliant flows after other flows in the sequence
 * @details Ensures that compliant traffic flows are processed last in the timeline
 */
#define NONCOMPLIANT_MODE_AFTER 1

/**
 * @def NONCOMPLIANT_MODE_AVERAGE
 * @brief Implements an average distribution between compliant and non-compliant flows
 * @details Balances the distribution of compliant and non-compliant traffic flows evenly
 */
#define NONCOMPLIANT_MODE_AVERAGE 2

/**
 * @def NONCOMPLIANT_MODE_EXTREME_COMPLIANT
 * @brief Extreme case scenario with a single compliant flow in the middle
 * @details Creates a test case with an isolated compliant flow surrounded by non-compliant flows
 */
#define NONCOMPLIANT_MODE_EXTREME_COMPLIANT 3

/**
 * @def NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT
 * @brief Extreme case scenario with a single non-compliant flow in the middle
 * @details Creates a test case with an isolated non-compliant flow surrounded by compliant flows
 */
#define NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT 4

/// @brief Define REDUCTION to recover the default configuration.

#ifdef REDUCTION

/**
 * @brief Parameters related to the basic simulation configuration
 *
 * @details This section defines the fundamental parameters that control the simulation environment,
 * including scale, timing, precision, and storage settings.
 */

/**
 * @def INITIAL_CONFIGURATION_TENANT_NUMBER
 * @brief Total number of tenants participating in the simulation
 */
#define INITIAL_CONFIGURATION_TENANT_NUMBER 100

/**
 * @def INITIAL_CONFIGURATION_SIMULATION_TIME
 * @brief Duration of the simulation in time units
 * @details Specifies the total runtime of the simulation in seconds
 */
#define INITIAL_CONFIGURATION_SIMULATION_TIME 5.0

/**
 * @def INITIAL_CONFIGURATION_ERROR
 * @brief Acceptable error margin for simulation calculations
 */
#define INITIAL_CONFIGURATION_ERROR 0.001

/**
 * @def INITIAL_CONFIGURATION_DATA_PATH
 * @brief Base directory path for storing simulation data
 */
#define INITIAL_CONFIGURATION_DATA_PATH "../data/main"

/**
 * @def INITIAL_CONFIGURATION_UNIT
 * @brief Base unit size for data measurements (1MB = 1048576 bytes)
 */
#define INITIAL_CONFIGURATION_UNIT 1048576

/**
 * @brief Traffic generation and control parameters
 *
 * @details Defines parameters that govern traffic generation patterns,
 * including rates, distributions, and packet characteristics.
 */

/**
 * @def INITIAL_CONFIGURATION_INPUT_RATE
 * @brief Base input traffic rate in bytes per second
 * @note Previous value was 167772160
 */
#define INITIAL_CONFIGURATION_INPUT_RATE (long)335544320

/**
 * @def INITIAL_CONFIGURATION_TRAFFIC_MODE
 * @brief Selected traffic generation mode
 * @see Refer to TRAFFIC_MODE definitions for available modes
 */
#define INITIAL_CONFIGURATION_TRAFFIC_MODE 0

/**
 * @def INITIAL_CONFIGURATION_MEAN
 * @brief Mean traffic rate for normal distribution
 */
#define INITIAL_CONFIGURATION_MEAN 120

/**
 * @def INITIAL_CONFIGURATION_STANDARD_DEVIATION
 * @brief Standard deviation for traffic distribution
 */
#define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40

/**
 * @def INITIAL_CONFIGURATION_PACKET_SIZE
 * @brief Standard packet size in bytes
 */
#define INITIAL_CONFIGURATION_PACKET_SIZE 512

/**
 * @brief Non-compliant traffic parameters
 *
 * @details Configuration parameters specific to non-compliant tenant behavior
 */

/**
 * @def INITIAL_CONFIGURATION_NONCOMPLIANT_MEAN
 * @brief Mean traffic rate for non-compliant tenants
 */
#define INITIAL_CONFIGURATION_NONCOMPLIANT_MEAN 155

/**
 * @def INITIAL_CONFIGURATION_NONCOMPLIANT_STANDARD_DEVIATION
 * @brief Standard deviation for non-compliant tenant traffic
 */
#define INITIAL_CONFIGURATION_NONCOMPLIANT_STANDARD_DEVIATION 10

/**
 * @def INITIAL_CONFIGURATION_NONCOMPLIANT_TENANT_NUMBER
 * @brief Number of tenants designated as non-compliant
 */
#define INITIAL_CONFIGURATION_NONCOMPLIANT_TENANT_NUMBER 50

/**
 * @def INITIAL_CONFIGURATION_STATE_R
 * @brief State transition rate parameter for traffic generation
 */
#define INITIAL_CONFIGURATION_STATE_R (float)0.5

/**
 * @def INITIAL_CONFIGURATION_NONCOMPLIANT_STATE_R
 * @brief State transition rate parameter for non-compliant traffic
 */
#define INITIAL_CONFIGURATION_NONCOMPLIANT_STATE_R (float)0.5

/**
 * @def INITIAL_CONFIGURATION_NONCOMPLIANT_MODE
 * @brief Operating mode for non-compliant traffic handling
 * @see Refer to NONCOMPLIANT_MODE definitions
 */
#define INITIAL_CONFIGURATION_NONCOMPLIANT_MODE 0

/**
 * @brief System threshold parameters
 *
 * @details Defines various buffer and queue size limits for the system.
 * These parameters are empirically determined and cannot be calculated.
 */

/**
 * @def INITIAL_CONFIGURATION_UPPER_LINK_BUFFER
 * @brief Maximum buffer size for upper link connections
 */
#define INITIAL_CONFIGURATION_UPPER_LINK_BUFFER 1

/**
 * @def INITIAL_CONFIGURATION_BUCKET_DEPTH
 * @brief Maximum depth of the token bucket buffer
 */
#define INITIAL_CONFIGURATION_BUCKET_DEPTH 512

/**
 * @def INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER
 * @brief Maximum buffer size for link queues
 */
#define INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER 100

/**
 * @brief Creates or updates a configuration file with default simulation parameters
 *
 * @details This function writes a structured INI configuration file with three main sections:
 *          - [simulation]: Basic simulation parameters
 *          - [traffic]: Traffic generation and control parameters
 *          - [threshold]: System threshold parameters
 *
 * @param filename The path and name of the configuration file to be created/modified
 *
 * @note The function will overwrite any existing file with the same name
 *
 * @warning Ensure proper write permissions in the target directory
 */
void reduction_inif_file(const char *filename)
{
    if (filename == NULL)
    {
        fprintf(stderr, "Error: NULL filename provided\n");
        return;
    }

    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\" for writing\n", filename);
        perror("File opening failed");
        return;
    }

    // Write [simulation] section
    if (fprintf(file, "[simulation]\n") < 0)
        goto write_error;
    if (fprintf(file, "tenant_number = %d\n", INITIAL_CONFIGURATION_TENANT_NUMBER) < 1)
        goto write_error;
    if (fprintf(file, "simulation_time = %lf\n", INITIAL_CONFIGURATION_SIMULATION_TIME) < 0)
        goto write_error;
    if (fprintf(file, "error = %f\n", INITIAL_CONFIGURATION_ERROR) < 0)
        goto write_error;
    fprintf(file, "data_path = %s\n", INITIAL_CONFIGURATION_DATA_PATH);
    if (fprintf(file, "unit = %ld\n\n", INITIAL_CONFIGURATION_UNIT) < 0)
        goto write_error;

    // Write [traffic] section
    if (fprintf(file, "[traffic]\n") < 0)
        goto write_error;
    if (fprintf(file, "input_rate = %ld\n", INITIAL_CONFIGURATION_INPUT_RATE) < 0)
        goto write_error;
    if (fprintf(file, "traffic_mode = %d\n", INITIAL_CONFIGURATION_TRAFFIC_MODE) < 0)
        goto write_error;
    if (fprintf(file, "mean = %d\n", INITIAL_CONFIGURATION_MEAN) < 0)
        goto write_error;
    if (fprintf(file, "standard_deviation = %d\n", INITIAL_CONFIGURATION_STANDARD_DEVIATION) < 0)
        goto write_error;
    if (fprintf(file, "packet_size = %d\n\n", INITIAL_CONFIGURATION_PACKET_SIZE) < 0)
        goto write_error;

    // Write non-compliant traffic parameters
    if (fprintf(file, "noncompliant_mean = %d\n", INITIAL_CONFIGURATION_NONCOMPLIANT_MEAN) < 0)
        goto write_error;
    if (fprintf(file, "noncompliant_standard_deviation = %d\n",
                INITIAL_CONFIGURATION_NONCOMPLIANT_STANDARD_DEVIATION) < 0)
        goto write_error;
    if (fprintf(file, "noncompliant_tenant_number = %d\n",
                INITIAL_CONFIGURATION_NONCOMPLIANT_TENANT_NUMBER) < 0)
        goto write_error;
    if (fprintf(file, "noncompliant_mode = %d\n", INITIAL_CONFIGURATION_NONCOMPLIANT_MODE) < 0)
        goto write_error;
    if (fprintf(file, "state_r = %f\n", INITIAL_CONFIGURATION_STATE_R) < 0)
        goto write_error;
    if (fprintf(file, "noncompliant_state_r = %f\n\n",
                INITIAL_CONFIGURATION_NONCOMPLIANT_STATE_R) < 0)
        goto write_error;

    // Write [threshold] section
    if (fprintf(file, "[threshold]\n") < 0)
        goto write_error;
    if (fprintf(file, "upper_queue_buffer = %ld\n", INITIAL_CONFIGURATION_UPPER_LINK_BUFFER) < 0)
        goto write_error;
    if (fprintf(file, "tau = %ld\n", INITIAL_CONFIGURATION_BUCKET_DEPTH) < 0)
        goto write_error;
    if (fprintf(file, "link_queue_buffer = %ld\n", INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER) < 0)
        goto write_error;

    fclose(file);
    return;

write_error:
    fprintf(stderr, "Error: Failed to write to file \"%s\"\n", filename);
    perror("Write operation failed");
    fclose(file);
    exit(EXIT_FAILURE);
    return;
}

#endif // REDUCTION

#ifdef CONFIG_H

/**
 * @brief Configuration structure containing all simulation parameters
 *
 * @details This structure holds all configuration parameters needed for the simulation,
 * organized into logical groups: basic simulation settings, traffic control parameters,
 * non-compliant traffic settings, and threshold configurations.
 */
typedef struct
{
    /**
     * @name Basic Simulation Parameters
     * @{
     */
    int tenant_number;         /**< Total number of tenants participating in the simulation */
    TIME_TYPE simulation_time; /**< Total duration of the simulation */
    double error;              /**< Acceptable error margin for simulation calculations */
    char *data_path;           /**< Path to data files and storage location */
    long unit;                 /**< Base unit for measurements (e.g., 1048576 for 1MB) */
    /** @} */

    /**
     * @name Traffic Control Parameters
     * @{
     */
    long input_rate;        /**< Base input traffic rate in bytes per second */
    int traffic_mode;       /**< Traffic generation mode (0: constant, 1: Gaussian, etc.) */
    int mean;               /**< Mean traffic rate for normal distribution */
    int standard_deviation; /**< Standard deviation for traffic distribution */
    int packet_size;        /**< Standard packet size in bytes */
    /** @} */

    /**
     * @name Non-compliant Traffic Parameters
     * @details Parameters specific to handling traffic that deviates from standard patterns
     * @{
     */
    int noncompliant_mean;               /**< Mean traffic rate for non-compliant flows */
    int noncompliant_standard_deviation; /**< Standard deviation for non-compliant traffic */
    int noncompliant_tenant_number;      /**< Number of tenants marked as non-compliant */
    int noncompliant_mode;               /**< Operating mode for non-compliant traffic handling */
    /** @} */

    /**
     * @name State Control Parameters
     * @details Parameters for finite state machine simulation of burst traffic
     * @{
     */
    double state_r;              /**< State transition rate for normal traffic */
    double noncompliant_state_r; /**< State transition rate for non-compliant traffic */
    /** @} */

    /**
     * @name Buffer and Queue Parameters
     * @details System threshold values for various buffers and queues
     * @{
     */
    long upper_queue_buffer; /**< Maximum size of upper queue buffer */
    long tau;                /**< Token bucket depth or threshold parameter */
    int link_queue_buffer;   /**< Maximum size of link queue buffer */
    /** @} */
} configuration;

/**
 * @brief Handler function to populate configuration structure from INI file
 *
 * @details Processes key-value pairs from an INI file and updates the configuration structure.
 *          Handles three sections: simulation, traffic, and threshold.
 *
 * @param config  Pointer to the configuration structure to be populated
 * @param section Section name from the INI file
 * @param name    Key name within the section
 * @param value   String value to be parsed
 *
 * @return int    Returns SUCCESS (0) if the parameter was handled successfully,
 *                FAILURE (-1) on error, or
 *                UNFOUND (1) if the parameter was not recognized
 *
 * @note All numeric conversions are bounds-checked
 */
static int handler(void *config, const char *section, const char *name, const char *value)
{
    if (!config || !section || !name || !value)
    {
        fprintf(stderr, "Error: NULL pointer passed to handler\n");
        return FAILURE;
    }

    configuration *pconfig = (configuration *)config;
    char *endptr;
    long temp_long;
    double temp_double;

// Helper macro for string comparison
#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

// Helper macro for safe string to long conversion
#define SAFE_STRTOL(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        errno = 0; /* 直接使用系統的 errno */                                            \
        temp_long = strtol(val, &endptr, 10);                                            \
        if (errno != 0 || *endptr != '\0' || temp_long < min || temp_long > max)         \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

// Helper macro for safe string to double conversion
#define SAFE_STRTOD(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        errno = 0; /* 直接使用系統的 errno */                                            \
        temp_double = strtod(val, &endptr);                                              \
        if (errno != 0 || *endptr != '\0' || temp_double < min || temp_double > max)     \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

    // Handle simulation section
    if (strcmp(section, "simulation") == 0)
    {
        if (MATCH("simulation", "tenant_number"))
        {
            SAFE_STRTOL(value, 1, INT_MAX);
            pconfig->tenant_number = (int)temp_long;
        }
        else if (MATCH("simulation", "simulation_time"))
        {
            SAFE_STRTOD(value, 0.0, DBL_MAX);
            pconfig->simulation_time = temp_double;
        }
        else if (MATCH("simulation", "error"))
        {
            SAFE_STRTOD(value, 0.0, 1.0);
            pconfig->error = (float)temp_double;
        }
        else if (MATCH("simulation", "data_path"))
        {
            if (strlen(value) > MAX_PATH_LENGTH)
            {
                fprintf(stderr, "Error: Path too long: %s\n", value);
                return FAILURE;
            }
            char *temp_str = strdup(value);
            if (!temp_str)
            {
                fprintf(stderr, "Error: Memory allocation failed for data_path\n");
                return FAILURE;
            }
            free(pconfig->data_path); // Free existing path if any
            pconfig->data_path = temp_str;
        }
        else if (MATCH("simulation", "unit"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX);
            pconfig->unit = temp_long;
        }
        else
            return UNFOUND;
    }

    // Handle traffic section
    else if (strcmp(section, "traffic") == 0)
    {
        if (MATCH("traffic", "input_rate"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX);
            pconfig->input_rate = temp_long;
        }
        else if (MATCH("traffic", "traffic_mode"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->traffic_mode = (int)temp_long;
        }
        else if (MATCH("traffic", "mean"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->mean = (int)temp_long;
        }
        else if (MATCH("traffic", "standard_deviation"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->standard_deviation = (int)temp_long;
        }
        else if (MATCH("traffic", "packet_size"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->packet_size = (int)temp_long;
        }
        // Non-compliant traffic parameters
        else if (MATCH("traffic", "noncompliant_mean"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->noncompliant_mean = (int)temp_long;
        }
        else if (MATCH("traffic", "noncompliant_standard_deviation"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->noncompliant_standard_deviation = (int)temp_long;
        }
        else if (MATCH("traffic", "noncompliant_tenant_number"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->noncompliant_tenant_number = (int)temp_long;
        }
        else if (MATCH("traffic", "noncompliant_mode"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->noncompliant_mode = (int)temp_long;
        }
        else if (MATCH("traffic", "state_r"))
        {
            SAFE_STRTOD(value, 0.0, 1.0);
            pconfig->state_r = temp_double;
        }
        else if (MATCH("traffic", "noncompliant_state_r"))
        {
            SAFE_STRTOD(value, 0.0, 1.0);
            pconfig->noncompliant_state_r = temp_double;
        }
        else
            return UNFOUND;
    }

    // Handle threshold section
    else if (strcmp(section, "threshold") == 0)
    {
        if (MATCH("threshold", "upper_queue_buffer"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX);
            pconfig->upper_queue_buffer = temp_long;
        }
        else if (MATCH("threshold", "tau"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX);
            pconfig->tau = temp_long;
        }
        else if (MATCH("threshold", "link_queue_buffer"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);
            pconfig->link_queue_buffer = (int)temp_long;
        }
        else
            return UNFOUND;
    }
    else
    {
        fprintf(stderr, "Warning: Unknown section: %s\n", section);
        return UNFOUND;
    }

#undef MATCH
#undef SAFE_STRTOL
#undef SAFE_STRTOD

    return SUCCESS;
}

/**
 * @brief Obtain the system capacity from a configuration file
 *
 * @details Reads the system capacity value from a specified file path.
 *          The file should contain a single positive number representing the capacity.
 *
 * @return double The system capacity value if successful
 * @return FAILED (-1.0) if an error occurs during file operations or invalid data
 *
 * @note The capacity value must be positive and within reasonable bounds
 *
 * @warning Ensure the CAPACITY_DATA_PATH macro is properly defined and the file exists
 *
 * Error conditions:
 * - File does not exist or cannot be opened
 * - File is empty or contains invalid data
 * - Capacity value is negative or exceeds system limits
 */
double obtain_capacity(void)
{
    FILE *file = NULL;
    double capacity = 0.0;
    const double MAX_CAPACITY = 1e12; // 1 Tbps as reasonable upper limit

    // Validate file path
    if (CAPACITY_DATA_PATH == NULL)
    {
        fprintf(stderr, "Error: CAPACITY_DATA_PATH is not defined\n");
        return FAILED;
    }

    // Open file
    file = fopen(CAPACITY_DATA_PATH, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Cannot open capacity file: %s\n", CAPACITY_DATA_PATH);
        perror("File opening failed");
        return FAILED;
    }

    // Read capacity value
    if (fscanf(file, "%lf", &capacity) != 1)
    {
        fprintf(stderr, "Error: Failed to read capacity value from %s\n", CAPACITY_DATA_PATH);
        perror("File reading failed");
        fclose(file);
        return FAILED;
    }

    // Validate capacity value
    if (capacity <= 0.0)
    {
        fprintf(stderr, "Error: Invalid capacity value: %lf (must be positive)\n", capacity);
        fclose(file);
        return FAILED;
    }

    if (capacity > MAX_CAPACITY)
    {
        fprintf(stderr, "Error: Capacity value too large: %lf (max: %lf)\n",
                capacity, MAX_CAPACITY);
        fclose(file);
        return FAILED;
    }

    // Check for additional unexpected data
    double dummy;
    if (fscanf(file, "%lf", &dummy) == 1)
    {
        fprintf(stderr, "Warning: Additional unexpected data found in %s\n",
                CAPACITY_DATA_PATH);
    }

    fclose(file);
    return capacity;
}

/**
 * @brief Helper function to write a string to file with error checking
 *
 * @param file File pointer
 * @param str String to write
 * @return int Returns 0 on success, -1 on failure
 */
static int try_write(FILE *file, const char *str)
{
    if (fputs(str, file) == EOF)
    {
        fprintf(stderr, "Error: Writing to file failed\n");
        return FAILED;
    }
    return 0;
}

/**
 * @brief Modify the INI file with the current configuration
 *
 * @details Writes the configuration structure to an INI format file.
 *          The file is organized in sections: simulation, traffic, and threshold.
 *
 * @param filename Path to the INI file to be written
 * @param config   Pointer to the configuration structure containing the data
 *
 * @return int Returns 0 on success, -1 on failure
 *
 * @note The function will overwrite the existing file if it exists
 *
 * @warning Ensure the config pointer is valid and all required fields are initialized
 */
int modify_ini_file(const char *filename, const configuration *config)
{
    FILE *file = NULL;
    int status = 0;

    // Input validation
    if (filename == NULL || config == NULL)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return FAILED;
    }

    // Open file with error handling
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\" for writing\n", filename);
        perror("File opening failed");
        return FAILED;
    }

// Helper macro for writing with error checking
#define WRITE_CHECK(fmt, ...)                                                    \
    if (fprintf(file, fmt, __VA_ARGS__) < 0)                                     \
    {                                                                            \
        fprintf(stderr, "Error: Writing to file failed at line %d\n", __LINE__); \
        status = FAILED;                                                         \
        goto cleanup;                                                            \
    }

    // Write [simulation] section
    try_write(file, "[simulation]\n");
    WRITE_CHECK("tenant_number = %d\n", config->tenant_number);
    WRITE_CHECK("simulation_time = %lf\n", config->simulation_time);
    WRITE_CHECK("error = %.6f\n", config->error);
    WRITE_CHECK("data_path = %s\n", config->data_path ? config->data_path : "");
    WRITE_CHECK("unit = %ld\n", config->unit);
    try_write(file, "\n");

    // Write [traffic] section
    try_write(file, "[traffic]\n");
    WRITE_CHECK("input_rate = %ld\n", config->input_rate);
    WRITE_CHECK("traffic_mode = %d\n", config->traffic_mode);
    WRITE_CHECK("mean = %d\n", config->mean);
    WRITE_CHECK("standard_deviation = %d\n", config->standard_deviation);
    WRITE_CHECK("packet_size = %d\n", config->packet_size);
    try_write(file, "\n");

    // Write non-compliant traffic parameters
    WRITE_CHECK("noncompliant_mean = %d\n", config->noncompliant_mean);
    WRITE_CHECK("noncompliant_standard_deviation = %d\n",
                config->noncompliant_standard_deviation);
    WRITE_CHECK("noncompliant_tenant_number = %d\n",
                config->noncompliant_tenant_number);
    WRITE_CHECK("noncompliant_mode = %d\n", config->noncompliant_mode);
    WRITE_CHECK("state_r = %.6f\n", config->state_r);
    WRITE_CHECK("noncompliant_state_r = %.6f\n", config->noncompliant_state_r);
    try_write(file, "\n");

    // Write [threshold] section
    try_write(file, "[threshold]\n");
    WRITE_CHECK("upper_queue_buffer = %ld\n", config->upper_queue_buffer);
    WRITE_CHECK("tau = %ld\n", config->tau);
    WRITE_CHECK("link_queue_buffer = %d\n", config->link_queue_buffer);

cleanup:
    if (file != NULL)
    {
        if (fclose(file) != 0)
        {
            fprintf(stderr, "Error: Failed to close file\n");
            perror("File closing failed");
            status = FAILED;
        }
    }

#undef WRITE_CHECK
    return status;
}

/**
 * @brief Display the current configuration settings
 *
 * @details Prints all configuration parameters in a formatted manner, organized into
 *          three main sections:
 *          - Simulation parameters (tenant number, time, error, data path, unit)
 *          - Traffic parameters (rates, modes, distributions)
 *          - Threshold parameters (queue buffers, tau)
 *
 * @param config Configuration structure containing all settings to be displayed
 *
 * @note For traffic_mode > TRAFFIC_MODE_GAUSSIAN, additional non-compliant
 *       traffic parameters will be displayed.
 * @note For traffic_mode > TRAFFIC_MODE_DENSITY, state_r will be displayed.
 * @note For traffic_mode >= TRAFFIC_MODE_DIFFERENT_R, noncompliant_state_r
 *       will be displayed.
 */
void show_configuration(const configuration config)
{
    print_equals_line();
    printf("- Simulation :\n");
    printf("| tenant number                   : %-d\n", config.tenant_number);
    printf("| simulation time                 : %-lf\n", config.simulation_time);
    printf("| error                           : %-f\n", config.error);
    printf("| data path                       : %-s\n", config.data_path);

    switch (config.unit)
    {
    case GBPS:
        printf("| unit                            : Gbps\n");
        break;
    case MBPS:
        printf("| unit                            : Mbps\n");
        break;
    case KBPS:
        printf("| unit                            : kbps\n");
        break;
    default:
        printf("| unit                            : Mbps\n");
    }

    printf("- Traffic :\n");
    printf("| input rate                      : %-ld\n", config.input_rate);
    printf("| traffic mode                    : %-d\n", config.traffic_mode);
    printf("| mean                            : %-d\n", config.mean);
    printf("| standard deviation              : %-d\n", config.standard_deviation);
    printf("| packet size                     : %-d\n", config.packet_size);

    if (config.traffic_mode > TRAFFIC_MODE_GAUSSIAN)
    {
        printf("| noncompliant mean               : %-d\n", config.noncompliant_mean);
        printf("| noncompliant standard deviation : %-d\n", config.noncompliant_standard_deviation);
        printf("| noncompliant tenant number      : %-d\n", config.noncompliant_tenant_number);
        printf("| noncompliant tenant number      : %-d\n", config.noncompliant_tenant_number);
        printf("| noncompliant mode               : %-d\n", config.noncompliant_mode);
    }

    if (config.traffic_mode > TRAFFIC_MODE_DENSITY)
        printf("| state r                         : %-f\n", config.state_r);
    if (config.traffic_mode >= TRAFFIC_MODE_DIFFERENT_R)
        printf("| noncompliant state r            : %-f\n", config.noncompliant_state_r);
    printf("- Threshold :\n");
    printf("| upper queue buffer              : %-ld\n", config.upper_queue_buffer);
    printf("| tau                             : %-ld\n", config.tau);
    printf("| link queue buffer               : %-d\n", config.link_queue_buffer);
    print_equals_line();
}

/**
 * @brief Calculates the Greatest Common Divisor (GCD) of two integers
 * @details Uses the Euclidean algorithm to find the largest positive integer
 *          that divides both input numbers without a remainder.
 *          The algorithm works by repeatedly applying the division algorithm
 *          until a remainder of 0 is obtained.
 *
 * Algorithm steps:
 * 1. If b equals 0, return a
 * 2. Otherwise, set b to the remainder of a divided by b
 * 3. Set a to the previous value of b
 * 4. Repeat steps 1-3 until b equals 0
 *
 * @param a First integer input
 * @param b Second integer input
 * @return The greatest common divisor of a and b
 *
 * @note The function assumes positive integer inputs
 * @note If both inputs are 0, the function returns 0
 */
int gcd(int a, int b)
{
    while (b != 0)
    {
        /** @brief Temporary variable to store b during swap */
        int temp = b;
        /** @brief Update b with remainder of a/b */
        b = a % b;
        /** @brief Update a with previous value of b */
        a = temp;
    }
    return a;
}

/**
 * @brief Determines if a tenant index is noncompliant based on configured mode
 * @details Checks whether a given tenant index should be considered noncompliant
 *          according to different distribution modes specified in configuration.
 *
 * Supported modes:
 * - NONCOMPLIANT_MODE_BEFORE: Noncompliant tenants are placed at the beginning
 * - NONCOMPLIANT_MODE_AFTER: Noncompliant tenants are placed at the end
 * - NONCOMPLIANT_MODE_AVERAGE: Noncompliant tenants are distributed evenly
 * - NONCOMPLIANT_MODE_EXTREME_COMPLIANT: All tenants except middle one are noncompliant
 * - NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT: Only middle tenant is noncompliant
 *
 * @param index The tenant index to check (0-based)
 * @param config Configuration structure containing mode and tenant parameters
 * @return 1 if tenant is noncompliant, 0 if compliant
 *
 * @warning For NONCOMPLIANT_MODE_AVERAGE, exits if GCD of tenant numbers is 1
 *
 * @note Default mode behaves same as NONCOMPLIANT_MODE_BEFORE
 *
 * Configuration requirements:
 * - tenant_number > 0
 * - noncompliant_tenant_number <= tenant_number
 * - Valid noncompliant_mode value
 */
int is_noncompliant_index(int index, const configuration config)
{
    switch (config.noncompliant_mode)
    {
    case NONCOMPLIANT_MODE_BEFORE:
        /** @brief Noncompliant if index is in the first portion */
        return (index >= (config.tenant_number - config.noncompliant_tenant_number));
        break;

    case NONCOMPLIANT_MODE_AFTER:
        /** @brief Noncompliant if index is in the last portion */
        return (index < config.noncompliant_tenant_number);
        break;

    case NONCOMPLIANT_MODE_AVERAGE:
        /**
         * @brief Distribute noncompliant tenants evenly
         * @details Uses GCD to ensure even distribution is possible
         */
        int gcd_ = gcd(config.tenant_number, config.noncompliant_tenant_number);
        if (gcd_ == 1)
        {
            printf(RED_ELOG "GCD of noncompliant_tenant_number and tenant number is 1!!\n" RESET);
            exit(EXIT_FAILURE);
        }
        int divisor = (int)(config.tenant_number / gcd_);
        return (index % divisor < (int)(config.noncompliant_tenant_number / gcd_));
        break;

    case NONCOMPLIANT_MODE_EXTREME_COMPLIANT:
        /** @brief All tenants except middle one are noncompliant */
        return (index != (int)(config.tenant_number / 2));

    case NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT:
        /** @brief Only middle tenant is noncompliant */
        return (index == (int)(config.tenant_number / 2));

    default:
        /** @brief Default behavior matches NONCOMPLIANT_MODE_BEFORE */
        return (index >= (config.tenant_number - config.noncompliant_tenant_number));
        break;
    }
}

#endif