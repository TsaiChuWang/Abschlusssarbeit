
#include <string.h>
#include <float.h>
#include <limits.h>
#include <errno.h>

#define CONFIG_H
#define REDUCTION

/** @brief Traffic mode for uniform distribution. */
#define TRAFFIC_MODE_UNIFORM 0
/** @brief Traffic mode for Gaussian distribution. */
#define TRAFFIC_MODE_GAUSSIAN 1
/** @brief Traffic mode for all non-compliant uniform distribution. */
#define TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM 2
/** @brief Traffic mode for non-compliant uniform distribution. */
#define TRAFFIC_MODE_NONCOMPLIANT_UNIFORM 3
/** @brief Traffic mode for density distribution. */
#define TRAFFIC_MODE_DENSITY 4
/** @brief Traffic mode for bursty traffic of all types. */
#define TRAFFIC_MODE_BURSTY_ALL 5
/** @brief Traffic mode for bursty compliant traffic. */
#define TRAFFIC_MODE_BURSTY_COMPLIANT 6
/** @brief Traffic mode for bursty non-compliant traffic. */
#define TRAFFIC_MODE_BURSTY_NONCOMPLIANT 7
/** @brief Traffic mode for different rates. */
#define TRAFFIC_MODE_DIFFERENT_R 8

#define TRAFFIC_MODE_ADVANCED_DIFFERENT 9

/**
 * @def NONCOMPLIANT_MODE_BEFORE
 * @brief Positions compliant flows before other flows in the sequence.
 * @details Ensures that compliant traffic flows are processed first in the timeline.
 */
#define NONCOMPLIANT_MODE_BEFORE 0
/**
 * @def NONCOMPLIANT_MODE_AFTER
 * @brief Positions compliant flows after other flows in the sequence.
 * @details Ensures that compliant traffic flows are processed last in the timeline.
 */
#define NONCOMPLIANT_MODE_AFTER 1
/**
 * @def NONCOMPLIANT_MODE_AVERAGE
 * @brief Implements an average distribution between compliant and non-compliant flows.
 * @details Balances the distribution of compliant and non-compliant traffic flows evenly.
 */
#define NONCOMPLIANT_MODE_AVERAGE 2
/**
 * @def NONCOMPLIANT_MODE_EXTREME_COMPLIANT
 * @brief Extreme case scenario with a single compliant flow in the middle.
 * @details Creates a test case with an isolated compliant flow surrounded by non-compliant flows.
 */
#define NONCOMPLIANT_MODE_EXTREME_COMPLIANT 3
/**
 * @def NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT
 * @brief Extreme case scenario with a single non-compliant flow in the middle.
 * @details Creates a test case with an isolated non-compliant flow surrounded by compliant flows.
 */
#define NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT 4

/// @brief Define REDUCTION to recover the default configuration.
#ifdef REDUCTION

/**
 * @brief Parameters related to the basic simulation configuration.
 *
 * @details This section defines the fundamental parameters that control the simulation environment,
 * including scale, timing, precision, and storage settings.
 */

#define INITIAL_CONFIGURATION_TENANT_NUMBER 100        ///< Number of tenants in the simulation.
#define INITIAL_CONFIGURATION_SIMULATION_TIME 5.0      ///< Total simulation time in seconds.
#define INITIAL_CONFIGURATION_ERROR 0.001              ///< Acceptable error margin for calculations.
#define INITIAL_CONFIGURATION_DATA_PATH "../data/main" ///< Path to the main data directory.
#define INITIAL_CONFIGURATION_UNIT 1048576             ///< Unit size in bytes (1 MB).
#define INITIAL_CONFIGURATION_RATIO (double)1.0        ///< Unit size in bytes (1 MB).

/**
 * @brief Traffic generation and control parameters.
 *
 * @details Defines parameters that govern traffic generation patterns,
 * including rates, distributions, and packet characteristics.
 */

#define INITIAL_CONFIGURATION_INPUT_RATE (long)335544320 ///< Input rate in bytes per second.
#define INITIAL_CONFIGURATION_TRAFFIC_MODE 0             ///< Traffic mode identifier (e.g., uniform, Gaussian).
#define INITIAL_CONFIGURATION_MEAN 120                   ///< Mean value for traffic generation.
#define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40      ///< Standard deviation for traffic generation.
#define INITIAL_CONFIGURATION_PACKET_SIZE 512            ///< Size of each packet in bytes.

/**
 * @brief Non-compliant traffic parameters.
 *
 * @details Configuration parameters specific to non-compliant tenant behavior.
 */

#define INITIAL_CONFIGURATION_NONCOMPLIANT_MEAN 155              ///< Mean value for non-compliant traffic generation.
#define INITIAL_CONFIGURATION_NONCOMPLIANT_STANDARD_DEVIATION 10 ///< Standard deviation for non-compliant traffic.
#define INITIAL_CONFIGURATION_NONCOMPLIANT_TENANT_NUMBER 50      ///< Number of non-compliant tenants.
#define INITIAL_CONFIGURATION_STATE_R (float)0.5                 ///< State R parameter for compliant traffic.
#define INITIAL_CONFIGURATION_NONCOMPLIANT_STATE_R (float)0.5    ///< State R parameter for non-compliant traffic.
#define INITIAL_CONFIGURATION_NONCOMPLIANT_MODE 0                ///< Mode identifier for non-compliant traffic behavior.

/**
 * @brief System threshold parameters.
 *
 * @details Defines various buffer and queue size limits for the system.
 * These parameters are empirically determined and cannot be calculated.
 */

#define INITIAL_CONFIGURATION_UPPER_LINK_BUFFER 10  ///< Upper limit for the link buffer.
#define INITIAL_CONFIGURATION_BUCKET_DEPTH 512      ///< Depth of the bucket for buffering.
#define INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER 100 ///< Size of the link queue buffer.

#define INITIAL_COMMON_CONFIGURATION_CSV_PATH "../configuration/csv/advanced.csv"

/**
 * @brief Writes simulation configuration to a specified INI file.
 *
 * @param filename The name of the file to write the configuration to.
 *
 * @details This function creates or overwrites an INI file with the simulation parameters,
 * including simulation settings, traffic generation parameters, non-compliant traffic parameters,
 * and system threshold parameters. If any errors occur during file operations, appropriate
 * error messages are printed to stderr.
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
    if (fprintf(file, "unit = %ld\n", INITIAL_CONFIGURATION_UNIT) < 0)
        goto write_error;
    if (fprintf(file, "ratio = %lf\n\n", INITIAL_CONFIGURATION_RATIO) < 0)
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

/**
 * @brief Writes common configuration settings to an INI file.
 *
 * This function creates or overwrites the specified INI file with
 * configuration settings for simulation, traffic, and threshold sections.
 *
 * @param filename The path to the INI file to be written.
 */
void reduction_inif_file_common_configuration(const char *filename)
{
    // Check if the provided filename is NULL.
    if (filename == NULL)
    {
        fprintf(stderr, "Error: NULL filename provided\n");
        return;
    }

    // Open the file for writing.
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\" for writing\n", filename);
        perror("File opening failed");
        return;
    }

    // Write [simulation] section
    if (fprintf(file, "[simulation]\n") < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "tenant_number = %d\n", INITIAL_CONFIGURATION_TENANT_NUMBER) < 1)
        goto write_error; // Handle write error
    if (fprintf(file, "simulation_time = %lf\n", INITIAL_CONFIGURATION_SIMULATION_TIME) < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "error = %f\n", INITIAL_CONFIGURATION_ERROR) < 0)
        goto write_error;                                               // Handle write error
    fprintf(file, "data_path = %s\n", INITIAL_CONFIGURATION_DATA_PATH); // No error check for data_path
    if (fprintf(file, "unit = %ld\n", INITIAL_CONFIGURATION_UNIT) < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "ratio = %lf\n", INITIAL_CONFIGURATION_RATIO) < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "csv_data_path = %s\n\n", INITIAL_COMMON_CONFIGURATION_CSV_PATH) < 0)
        goto write_error; // Handle write error

    // Write [traffic] section
    if (fprintf(file, "[traffic]\n") < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "input_rate = %ld\n", INITIAL_CONFIGURATION_INPUT_RATE) < 0)
        goto write_error; // Handle write error

    // Write [threshold] section
    if (fprintf(file, "[threshold]\n") < 0)
        goto write_error; // Handle write error
    if (fprintf(file, "link_queue_buffer = %ld\n", INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER) < 0)
        goto write_error; // Handle write error

    fclose(file); // Close the file successfully
    return;       // Exit the function normally

write_error:
    fprintf(stderr, "Error: Failed to write to file \"%s\"\n", filename);
    perror("Write operation failed"); // Print the error for the write operation
    fclose(file);                     // Close the file if an error occurred
    exit(EXIT_FAILURE);               // Exit with failure status
}

#endif // REDUCTION

#ifdef CONFIG_H

/**
 * @brief Configuration structure for simulation parameters.
 *
 * This structure holds all necessary parameters for the simulation,
 * organized into groups: basic settings, traffic control, non-compliant traffic,
 * and threshold configurations.
 */
typedef struct
{
    /** @name Basic Simulation Parameters */
    int tenant_number;         /**< Number of tenants in the simulation. */
    TIME_TYPE simulation_time; /**< Duration of the simulation. */
    double error;              /**< Acceptable error margin. */
    char *data_path;           /**< Path to data files. */
    long unit;                 /**< Measurement unit (e.g., 1048576 for 1MB). */
    double ratio;

    /** @name Traffic Control Parameters */
    long input_rate;        /**< Input traffic rate in bytes per second. */
    int traffic_mode;       /**< Traffic generation mode (0: constant, 1: Gaussian). */
    int mean;               /**< Mean traffic rate. */
    int standard_deviation; /**< Standard deviation for traffic. */
    int packet_size;        /**< Packet size in bytes. */

    /** @name Non-compliant Traffic Parameters */
    int noncompliant_mean;               /**< Mean for non-compliant traffic. */
    int noncompliant_standard_deviation; /**< Std deviation for non-compliant traffic. */
    int noncompliant_tenant_number;      /**< Non-compliant tenant count. */
    int noncompliant_mode;               /**< Mode for non-compliant traffic. */

    /** @name State Control Parameters */
    double state_r;              /**< Transition rate for normal traffic. */
    double noncompliant_state_r; /**< Transition rate for non-compliant traffic. */

    /** @name Buffer and Queue Parameters */
    long upper_queue_buffer; /**< Max upper queue buffer size. */
    long tau;                /**< Token bucket depth. */
    int link_queue_buffer;   /**< Max link queue buffer size. */
} configuration;

typedef struct
{
    int tenant_number;         /**< Number of tenants in the simulation. */
    TIME_TYPE simulation_time; /**< Duration of the simulation. */
    double error;              /**< Acceptable error margin. */
    char *data_path;           /**< Path to data files. */
    long unit;                 /**< Measurement unit (e.g., 1048576 for 1MB). */
    double ratio;
    char *csv_data_path;

    /** @name Traffic Control Parameters */
    long input_rate; /**< Input traffic rate in bytes per second. */

    /** @name Buffer and Queue Parameters */
    int link_queue_buffer; /**< Max link queue buffer size. */
} common_configuration;

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

// Safe string to long conversion
#define SAFE_STRTOL(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        temp_long = strtol(val, &endptr, 10);                                            \
        if (*endptr != '\0' || temp_long < min || temp_long > max)                       \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

// Safe string to double conversion
#define SAFE_STRTOD(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        temp_double = strtod(val, &endptr);                                              \
        if (*endptr != '\0' || temp_double < min || temp_double > max)                   \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

    // Handle sections and parameters
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
            pconfig->error = temp_double;
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
            // free(pconfig->data_path); // Free existing path if any
            pconfig->data_path = temp_str;
        }
        else if (MATCH("simulation", "unit"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX);
            pconfig->unit = temp_long;
        }
        else if (MATCH("simulation", "ratio"))
        {
            SAFE_STRTOD(value, 0.0, DBL_MAX);
            pconfig->ratio = temp_double;
        }
        else
        {
            return UNFOUND;
        }
    }
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
        {
            return UNFOUND;
        }
    }
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
        {
            return UNFOUND;
        }
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
 * @brief Handles common configuration settings from a given section and name.
 *
 * This function processes configuration values for various sections such as
 * "simulation", "traffic", and "threshold". It validates and assigns values
 * to the corresponding fields in the common_configuration structure.
 *
 * @param config Pointer to a common_configuration structure.
 * @param section The section of the configuration (e.g., "simulation").
 * @param name The name of the configuration parameter (e.g., "tenant_number").
 * @param value The value associated with the parameter.
 * @return SUCCESS if the operation was successful, FAILURE if an error occurred,
 *         or UNFOUND if the parameter was not recognized.
 */
static int handler_common_configuration(void *config, const char *section, const char *name, const char *value)
{
    // Check for NULL pointers in the input parameters.
    if (!config || !section || !name || !value)
    {
        fprintf(stderr, "Error: NULL pointer passed to handler\n");
        return FAILURE;
    }

    common_configuration *pconfig = (common_configuration *)config; // Cast config to common_configuration
    char *endptr;                                                   // Pointer for string conversion
    long temp_long;                                                 // Temporary variable for long values
    double temp_double;                                             // Temporary variable for double values

// Helper macro for string comparison
#define MATCH(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

// Safe string to long conversion
#define SAFE_STRTOL(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        temp_long = strtol(val, &endptr, 10);                                            \
        if (*endptr != '\0' || temp_long < min || temp_long > max)                       \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

// Safe string to double conversion
#define SAFE_STRTOD(val, min, max)                                                       \
    do                                                                                   \
    {                                                                                    \
        temp_double = strtod(val, &endptr);                                              \
        if (*endptr != '\0' || temp_double < min || temp_double > max)                   \
        {                                                                                \
            fprintf(stderr, "Error: Invalid value for %s.%s: %s\n", section, name, val); \
            return FAILURE;                                                              \
        }                                                                                \
    } while (0)

    // Handle sections and parameters
    if (strcmp(section, "simulation") == 0)
    {
        // Process simulation parameters
        if (MATCH("simulation", "tenant_number"))
        {
            SAFE_STRTOL(value, 1, INT_MAX);          // Validate and convert tenant_number
            pconfig->tenant_number = (int)temp_long; // Assign value
        }
        else if (MATCH("simulation", "simulation_time"))
        {
            SAFE_STRTOD(value, 0.0, DBL_MAX);       // Validate and convert simulation_time
            pconfig->simulation_time = temp_double; // Assign value
        }
        else if (MATCH("simulation", "error"))
        {
            SAFE_STRTOD(value, 0.0, 1.0); // Validate and convert error
            pconfig->error = temp_double; // Assign value
        }
        else if (MATCH("simulation", "data_path"))
        {
            // Validate data_path length
            if (strlen(value) > MAX_PATH_LENGTH)
            {
                fprintf(stderr, "Error: Path too long: %s\n", value);
                return FAILURE;
            }
            char *temp_str = strdup(value); // Duplicate string for data_path
            if (!temp_str)
            {
                fprintf(stderr, "Error: Memory allocation failed for data_path\n");
                return FAILURE;
            }
            // free(pconfig->data_path); // Free existing path if any
            pconfig->data_path = temp_str; // Assign new path
        }
        else if (MATCH("simulation", "unit"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX); // Validate and convert unit
            pconfig->unit = temp_long;       // Assign value
        }
        else if (MATCH("simulation", "ratio"))
        {
            SAFE_STRTOD(value, 0.0, DBL_MAX); // Validate and convert ratio
            pconfig->ratio = temp_double;     // Assign value
        }
        else if (MATCH("simulation", "csv_data_path"))
        {
            // Validate data_path length
            if (strlen(value) > MAX_PATH_LENGTH)
            {
                fprintf(stderr, "Error: Path too long: %s\n", value);
                return FAILURE;
            }
            char *temp_str = strdup(value); // Duplicate string for data_path
            if (!temp_str)
            {
                fprintf(stderr, "Error: Memory allocation failed for data_path\n");
                return FAILURE;
            }
            // free(pconfig->data_path); // Free existing path if any
            pconfig->csv_data_path = temp_str; // Assign new path
        }
        else
        {
            return UNFOUND; // Parameter not found
        }
    }
    else if (strcmp(section, "traffic") == 0)
    {
        // Process traffic parameters
        if (MATCH("traffic", "input_rate"))
        {
            SAFE_STRTOL(value, 0, LONG_MAX); // Validate and convert input_rate
            pconfig->input_rate = temp_long; // Assign value
        }
        else
        {
            return UNFOUND; // Parameter not found
        }
    }
    else if (strcmp(section, "threshold") == 0)
    {
        // Process threshold parameters
        if (MATCH("threshold", "link_queue_buffer"))
        {
            SAFE_STRTOL(value, 0, INT_MAX);              // Validate and convert link_queue_buffer
            pconfig->link_queue_buffer = (int)temp_long; // Assign value
        }
        else
        {
            return UNFOUND; // Parameter not found
        }
    }
    else
    {
        fprintf(stderr, "Warning: Unknown section: %s\n", section); // Unknown section warning
        return UNFOUND;                                             // Section not found
    }

// Undefine macros after use
#undef MATCH
#undef SAFE_STRTOL
#undef SAFE_STRTOD

    return SUCCESS; // Successful operation
}

/**
 * @brief Obtain the system capacity from a configuration file
 *
 * @details Reads the system capacity value from a specified file path.
 *          The file should contain a single positive number representing the capacity.
 *
 * @return double The system capacity value if successful
 * @return FAILURE (-1.0) if an error occurs during file operations or invalid data
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

    file = fopen(CAPACITY_DATA_PATH, "r");
    if (file == NULL)
    {
        printf("Cannot read file %s\n", CAPACITY_DATA_PATH);
        perror("Error opening file " CAPACITY_DATA_PATH);
        return FAILURE;
    }

    // Read capacity value
    if (fscanf(file, "%lf", &capacity) != 1)
    {
        fprintf(stderr, "Error: Failed to read capacity value from %s\n", CAPACITY_DATA_PATH);
        perror("File reading failed");
        fclose(file);
        return FAILURE;
    }

    // Validate capacity value
    if (capacity <= 0.0)
    {
        fprintf(stderr, "Error: Invalid capacity value: %lf (must be positive)\n", capacity);
        fclose(file);
        return FAILURE;
    }

    if (capacity > MAX_CAPACITY)
    {
        fprintf(stderr, "Error: Capacity value too large: %lf (max: %lf)\n",
                capacity, MAX_CAPACITY);
        fclose(file);
        return FAILURE;
    }

    fclose(file);
    return capacity;
}

/**
 * @brief Attempts to write a string to a file.
 *
 * This function writes the provided string to the specified file.
 * If the writing process fails, an error message is printed.
 *
 * @param file Pointer to the file where the string will be written.
 * @param str The string to be written to the file.
 * @return 0 on success, or FAILURE (-1) if an error occurs.
 */
static int try_write(FILE *file, const char *str)
{
    if (fputs(str, file) == EOF)
    {
        fprintf(stderr, "Error: Writing to file failed\n");
        return FAILURE;
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
        return FAILURE;
    }

    // Open file with error handling
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\" for writing\n", filename);
        perror("File opening failed");
        return FAILURE;
    }

    // Helper macro for writing with error checking
#define WRITE_CHECK(fmt, ...)                                                    \
    if (fprintf(file, fmt, __VA_ARGS__) < 0)                                     \
    {                                                                            \
        fprintf(stderr, "Error: Writing to file failed at line %d\n", __LINE__); \
        status = FAILURE;                                                        \
        goto cleanup;                                                            \
    }

    // Write [simulation] section
    try_write(file, "[simulation]\n");
    WRITE_CHECK("tenant_number = %d\n", config->tenant_number);
    WRITE_CHECK("simulation_time = %lf\n", config->simulation_time);
    WRITE_CHECK("error = %.6f\n", config->error);
    WRITE_CHECK("data_path = %s\n", config->data_path ? config->data_path : "");
    WRITE_CHECK("unit = %ld\n", config->unit);
    WRITE_CHECK("ratio = %lf\n", config->ratio);
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
    WRITE_CHECK("noncompliant_standard_deviation = %d\n", config->noncompliant_standard_deviation);
    WRITE_CHECK("noncompliant_tenant_number = %d\n", config->noncompliant_tenant_number);
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
            status = FAILURE;
        }
    }

#undef WRITE_CHECK
    return status;
}

/**
 * @brief Modifies an INI file with common configuration settings.
 *
 * This function writes the provided configuration settings to the specified INI file.
 * It creates or overwrites the existing file and organizes the settings into sections.
 *
 * @param filename The path to the INI file to be modified.
 * @param config Pointer to a common_configuration structure containing the settings.
 * @return SUCCESS if the operation was successful, FAILURE if an error occurred.
 */
int modify_ini_file_common_configuration(const char *filename, const common_configuration *config)
{
    FILE *file = NULL; // File pointer for the INI file
    int status = 0;    // Status variable to track success or failure

    // Input validation
    if (filename == NULL || config == NULL)
    {
        fprintf(stderr, "Error: Invalid parameters (NULL pointer)\n");
        return FAILURE; // Return failure if parameters are invalid
    }

    // Open file with error handling
    file = fopen(filename, "w");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\" for writing\n", filename);
        perror("File opening failed"); // Print error details
        return FAILURE;                // Return failure if file can't be opened
    }

// Helper macro for writing with error checking
#define WRITE_CHECK(fmt, ...)                                                    \
    if (fprintf(file, fmt, __VA_ARGS__) < 0)                                     \
    {                                                                            \
        fprintf(stderr, "Error: Writing to file failed at line %d\n", __LINE__); \
        status = FAILURE;                                                        \
        goto cleanup;                                                            \
    }

    // Write [simulation] section
    try_write(file, "[simulation]\n");                                           // Write section header
    WRITE_CHECK("tenant_number = %d\n", config->tenant_number);                  // Write tenant_number
    WRITE_CHECK("simulation_time = %lf\n", config->simulation_time);             // Write simulation_time
    WRITE_CHECK("error = %.6f\n", config->error);                                // Write error
    WRITE_CHECK("data_path = %s\n", config->data_path ? config->data_path : ""); // Write data_path
    WRITE_CHECK("unit = %ld\n", config->unit);                                   // Write unit
    WRITE_CHECK("ratio = %lf\n", config->ratio);                                 // Write ratio
    WRITE_CHECK("csv_data_path = %s\n", config->csv_data_path);                  // Write ratio
    try_write(file, "\n");                                                       // Add a newline after the section

    // Write [traffic] section
    try_write(file, "[traffic]\n");                        // Write section header
    WRITE_CHECK("input_rate = %ld\n", config->input_rate); // Write input_rate
    try_write(file, "\n");                                 // Add a newline after the section

    // Write [threshold] section
    try_write(file, "[threshold]\n");                                   // Write section header
    WRITE_CHECK("link_queue_buffer = %d\n", config->link_queue_buffer); // Write link_queue_buffer

cleanup:
    // Cleanup and close the file
    if (file != NULL)
    {
        if (fclose(file) != 0)
        {
            fprintf(stderr, "Error: Failed to close file\n");
            perror("File closing failed"); // Print error details
            status = FAILURE;              // Update status to failure
        }
    }

#undef WRITE_CHECK // Undefine the macro after use
    return status; // Return the final status
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
    printf("| ratio                           : %-lf\n", config.ratio);

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
 * @brief Displays the common configuration settings.
 *
 * This function prints the configuration settings to the standard output,
 * organizing them into sections for better readability. It includes details
 * about simulation parameters, traffic settings, and threshold values.
 *
 * @param config The common_configuration structure containing the settings to display.
 */
void show_configuration_common_configuration(const common_configuration config)
{
    print_equals_line();                                                          // Print a separator line
    printf("- Simulation :\n");                                                   // Header for the simulation section
    printf("| tenant number                   : %-d\n", config.tenant_number);    // Display tenant number
    printf("| simulation time                 : %-lf\n", config.simulation_time); // Display simulation time
    printf("| error                           : %-f\n", config.error);            // Display error value
    printf("| data path                       : %-s\n", config.data_path);        // Display data path

    // Display unit based on its value
    switch (config.unit)
    {
    case GBPS:
        printf("| unit                            : Gbps\n"); // Gigabits per second
        break;
    case MBPS:
        printf("| unit                            : Mbps\n"); // Megabits per second
        break;
    case KBPS:
        printf("| unit                            : kbps\n"); // Kilobits per second
        break;
    default:
        printf("| unit                            : Mbps\n"); // Default to Mbps if unknown
    }

    printf("| ratio                           : %-lf\n", config.ratio);        // Display ratio
    printf("| csv data path                   : %-s\n", config.csv_data_path); // Display data path

    printf("- Traffic :\n");                                                 // Header for the traffic section
    printf("| input rate                      : %-ld\n", config.input_rate); // Display input rate

    printf("| link queue buffer               : %-d\n", config.link_queue_buffer); // Display link queue buffer
    print_equals_line();                                                           // Print a separator line
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
        /** Noncompliant if index is in the first portion */
        return (index >= (config.tenant_number - config.noncompliant_tenant_number));

    case NONCOMPLIANT_MODE_AFTER:
        /** Noncompliant if index is in the last portion */
        return (index < config.noncompliant_tenant_number);

    case NONCOMPLIANT_MODE_AVERAGE:
        /**
         * Distribute noncompliant tenants evenly.
         * Uses GCD to ensure even distribution is possible.
         */
        int gcd_ = gcd(config.tenant_number, config.noncompliant_tenant_number);
        if (gcd_ == 1)
        {
            printf(RED_ELOG "GCD of noncompliant_tenant_number and tenant number is 1!!\n" RESET);
            exit(EXIT_FAILURE);
        }
        int divisor = (int)(config.tenant_number / gcd_);
        return (index % divisor < (int)(config.noncompliant_tenant_number / gcd_));

    case NONCOMPLIANT_MODE_EXTREME_COMPLIANT:
        /** All tenants except the middle one are noncompliant */
        return (index != (int)(config.tenant_number / 2));

    case NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT:
        /** Only the middle tenant is noncompliant */
        return (index == (int)(config.tenant_number / 2));

    default:
        /** Default behavior matches NONCOMPLIANT_MODE_BEFORE */
        return (index >= (config.tenant_number - config.noncompliant_tenant_number));
    }
}

/**
 * @brief Tests the configuration handling functionality.
 *
 * This function performs a series of operations to test loading,
 * modifying, and displaying configuration settings from an INI file.
 *
 * @param filename The name of the INI file to be processed.
 */
void test_configuration_h(const char *filename)
{
    configuration config;

    printf("===== handler =====\n");
    if (ini_parse(filename, handler, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }
    show_configuration(config);

    printf("===== reduction_inif_file =====\n");
    reduction_inif_file(filename);
    if (ini_parse(filename, handler, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }
    show_configuration(config);

    printf("===== obtain_capacity =====\n");
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "python3 ../python/capacity.py %s 0", filename);
    system(command);
    printf("capacity = %f\n", obtain_capacity());

    printf("===== modify_ini_file =====\n");
    config.tau = 114514;
    int status = modify_ini_file(filename, &config);
    if (status == FAILURE)
    {
        printf(RED_ELOG "modify_ini_file failed\n" RESET);
        exit(FAILURE);
    }
    if (ini_parse(filename, handler, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename);
        exit(EXIT_FAILURE);
    }
    show_configuration(config);

    reduction_inif_file(filename);
}

/**
 * @brief Tests the configuration handling for common_configuration.
 *
 * This function parses a configuration file, displays its contents,
 * modifies the configuration, and then verifies the changes by reloading
 * and displaying the updated configuration.
 *
 * @param filename The path to the configuration file to be tested.
 */
void test_configuration_h_common_configuration(const char *filename)
{
    common_configuration config; // Structure to hold configuration settings

    printf("===== handler =====\n");
    // Parse the INI file and load configurations into 'config'
    if (ini_parse(filename, handler_common_configuration, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename); // Error message if loading fails
        exit(EXIT_FAILURE);                                             // Exit if configuration loading fails
    }
    show_configuration_common_configuration(config); // Display the loaded configuration

    printf("===== reduction_inif_file =====\n");
    reduction_inif_file_common_configuration(filename); // Reduce the INI file
    // Re-parse the INI file after reduction
    if (ini_parse(filename, handler_common_configuration, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename); // Error message if loading fails
        exit(EXIT_FAILURE);                                             // Exit if configuration loading fails
    }
    show_configuration_common_configuration(config); // Display the reduced configuration

    printf("===== obtain_capacity =====\n");
    char command[MAX_COMMAND_LENGTH];
    // Uncomment the following line to execute a Python script for capacity calculation
    // sprintf(command, "python3 ../python/capacity.py %s 0", filename);
    // system(command); // Execute the command
    printf("capacity = %f\n", obtain_capacity()); // Display the obtained capacity

    printf("===== modify_ini_file =====\n");
    config.link_queue_buffer = 114514;                                    // Modify a specific field in the configuration
    int status = modify_ini_file_common_configuration(filename, &config); // Save the modified configuration
    if (status == FAILURE)
    {
        printf(RED_ELOG "modify_ini_file failed\n" RESET); // Error message if modification fails
        exit(FAILURE);                                     // Exit if modification fails
    }
    // Re-parse the INI file after modification
    if (ini_parse(filename, handler_common_configuration, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", filename); // Error message if loading fails
        exit(EXIT_FAILURE);                                             // Exit if configuration loading fails
    }
    show_configuration_common_configuration(config); // Display the updated configuration

    reduction_inif_file_common_configuration(filename); // Final reduction of the INI file

    // Call the function to test the configuration handling
    // test_configuration_h_common_configuration(configuration_path);
}

#endif

#define CSV_CONFIGURSTION_H

#ifdef CSV_CONFIGURSTION_H

#define MAX_LINE_LENGTH 2048

typedef struct
{
    int index;              // Index of the row configuration
    int traffic_mode;       // Mode of traffic (e.g., 0 for normal, 1 for high, etc.)
    int mean;               // Mean value for traffic generation
    int standard_deviation; // Standard deviation for traffic generation
    int number;             // Number of packets or connections
    int packet_size;        // Size of packets in bytes
    int real_traffic;       // Actual traffic value
    double state_r;         // State variable (could represent a ratio or probability)
    int FIFO_queue_buffer;  // Size of the FIFO queue buffer
    long tau;               // Time constant or delay in some context

    int start_index; // Starting index for processing
    int end_index;   // Ending index for processing

    double loss;
} row_configuration;

/**
 * @brief Structure to hold the configuration for CSV data.
 *
 * This structure contains the total number of rows (kind_number),
 * the expected number of fields in each row (fields_number),
 * an array of row_configuration structures, and common configuration settings.
 */
typedef struct
{
    int kind_number;   // Total number of row configurations
    int fields_number; // Expected number of fields in each row

    int tenant_number;

    row_configuration *rows;     // Pointer to an array of row_configuration
    common_configuration config; // Common configuration settings

    double average_loss;
    double compliant_loss;
    double non_compliant_loss;
    double uniform_loss;
    double burst_loss;
} csv_configuration;

/**
 * @brief Counts the number of rows in a CSV file.
 *
 * This function opens a specified CSV file and counts the number of rows
 * based on newline characters. It handles the case where the last line
 * may not end with a newline.
 *
 * @param filename The path to the CSV file to be read.
 * @return The number of rows in the CSV file, or -1 if the file cannot be opened.
 */
int count_csv_rows(const char *filename)
{
    FILE *file = fopen(filename, "r"); // Open the file for reading
    if (file == NULL)
    {
        printf(RED_ELOG "Cannot open file: %s\n" RESET, filename); // Error message if file can't be opened
        return -1;                                                 // Return -1 to indicate an error
    }

    int row_count = 0; // Initialize row count
    char ch;           // Variable to store each character read from the file

    // Read each character until the end of the file
    while ((ch = fgetc(file)) != EOF)
    {
        if (ch == '\n')
        {
            row_count++; // Increment row count for each newline character
        }
    }

    // Check if the last line does not end with a newline
    fseek(file, -1, SEEK_END); // Move the file pointer to the last character
    if (ftell(file) > 0)
    {                     // Ensure the file is not empty
        ch = fgetc(file); // Read the last character
        if (ch != '\n')
        {
            row_count++; // Increment row count if the last line does not end with a newline
        }
    }

    fclose(file);     // Close the file
    return row_count; // Return the total row count
}

/**
 * @brief Counts the number of columns in a CSV file.
 *
 * This function opens a specified CSV file and counts the number of columns
 * based on the number of commas in the first line. It assumes that the first
 * line contains the header or the data format.
 *
 * @param filename The path to the CSV file to be read.
 * @return The number of columns in the CSV file, or -1 if the file cannot be opened.
 *         Returns 0 if the file is empty or cannot read the first line.
 */
int count_csv_columns(const char *filename)
{
    FILE *file = fopen(filename, "r"); // Open the file for reading
    if (file == NULL)
    {
        printf(RED_ELOG "Cannot open file: %s\n" RESET, filename); // Error message if file can't be opened
        return -1;                                                 // Return -1 to indicate an error
    }

    char line[MAX_LINE_LENGTH]; // Buffer to hold the first line of the CSV file

    // Read the first line from the file
    if (fgets(line, sizeof(line), file) == NULL)
    {
        fclose(file); // Close the file if reading fails
        return 0;     // Return 0 if the file is empty or cannot read the first line
    }

    // Remove the newline character from the end of the line, if present
    line[strcspn(line, "\n")] = '\0';

    int comma_count = 0; // Initialize comma count
    // Count the number of commas in the line
    for (int i = 0; line[i] != '\0'; i++)
    {
        if (line[i] == ',')
        {
            comma_count++; // Increment comma count for each comma found
        }
    }

    fclose(file); // Close the file

    return comma_count + 1; // Return the number of columns (commas + 1)
}

/**
 * @brief Reads a specific row from a CSV file by index.
 *
 * This function opens the specified CSV file and reads it line by line
 * until it reaches the specified index. It returns the contents of that
 * row as a dynamically allocated string. The caller is responsible for
 * freeing the allocated memory.
 *
 * @param filename The path to the CSV file to read.
 * @param index The zero-based index of the row to read.
 * @return A pointer to a dynamically allocated string containing the row data,
 *         or NULL if the file cannot be opened or if the row does not exist.
 */
char *read_csv_row_by_index(const char *filename, int index)
{
    FILE *file = fopen(filename, "r"); // Open the file for reading
    if (file == NULL)
    {
        printf("Cannot open file: %s\n", filename); // Error message if file can't be opened
        return NULL;                                // Return NULL to indicate an error
    }

    char line[MAX_LINE_LENGTH]; // Buffer to hold each line of the CSV file
    int current_index = 0;      // Initialize the current index counter

    // Read lines until the specified index is reached or end of file
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (current_index == index)
        {                 // Check if the current index matches the specified index
            fclose(file); // Close the file

            line[strcspn(line, "\n")] = '\0'; // Remove the newline character

            // Allocate memory for the result string
            char *result = malloc(strlen(line) + 1);
            if (result == NULL)
            {
                printf("Memory allocation failed.\n"); // Handle memory allocation failure
                return NULL;                           // Return NULL if allocation fails
            }
            strcpy(result, line); // Copy the line into the result string
            return result;        // Return the result
        }
        current_index++; // Increment the index counter
    }

    fclose(file); // Close the file if the specified index is not found
    return NULL;  // Return NULL if the row does not exist
}

/**
 * @brief Splits a CSV row string into an array of fields.
 *
 * This function takes a string representing a row from a CSV file and splits
 * it into individual fields based on the comma delimiter. Leading and trailing
 * whitespace is removed from each field.
 *
 * @param row_string The CSV row string to split.
 * @param fields_number The expected number of fields in the row.
 * @return A pointer to an array of strings (fields), or NULL if an error occurs.
 *         The caller is responsible for freeing the allocated memory.
 */
char **split_csv_row(const char *row_string, int fields_number)
{
    if (row_string == NULL || fields_number <= 0)
    {
        return NULL; // Return NULL for invalid input
    }

    // Allocate memory for the array of fields
    char **fields = malloc(fields_number * sizeof(char *));
    if (fields == NULL)
    {
        return NULL; // Return NULL if memory allocation fails
    }

    // Initialize all fields to NULL
    for (int i = 0; i < fields_number; i++)
    {
        fields[i] = NULL;
    }

    // Create a copy of the row string to avoid modifying the original
    char *row_copy = malloc(strlen(row_string) + 1);
    if (row_copy == NULL)
    {
        free(fields); // Free previously allocated memory
        return NULL;  // Return NULL if memory allocation fails
    }
    strcpy(row_copy, row_string);

    // Split the row string into tokens using comma as the delimiter
    char *token = strtok(row_copy, ",");
    int field_index = 0;

    while (token != NULL && field_index < fields_number)
    {
        // Trim leading whitespace
        while (*token == ' ' || *token == '\t')
            token++;

        // Trim trailing whitespace
        int len = strlen(token);
        while (len > 0 && (token[len - 1] == ' ' || token[len - 1] == '\t'))
        {
            token[len - 1] = '\0';
            len--;
        }

        // Allocate memory for the field and copy the token
        fields[field_index] = malloc(strlen(token) + 1);
        if (fields[field_index] != NULL)
        {
            strcpy(fields[field_index], token);
        }

        field_index++;
        token = strtok(NULL, ","); // Get the next token
    }

    free(row_copy); // Free the copy of the row string
    return fields;  // Return the array of fields
}

#define TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION 0
#define TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL 1

/**
 * @brief Creates a row_configuration from an array of string fields.
 *
 * This function populates a row_configuration structure using values
 * parsed from the provided fields. It updates the start_index to
 * reflect the next available index after the current configuration.
 *
 * @param fields An array of strings containing configuration values.
 * @param start_index A pointer to the current start index, which will be updated.
 * @return A populated row_configuration structure.
 */
row_configuration create_row_configuration(char **fields, int *start_index)
{
    row_configuration row;
    char *endptr;

    // Convert string fields to respective data types
    row.index = atoi(fields[0]);
    row.traffic_mode = atoi(fields[1]);
    row.mean = atoi(fields[2]);
    row.standard_deviation = atoi(fields[3]);
    row.number = atoi(fields[4]);
    row.packet_size = atoi(fields[5]);
    row.real_traffic = atoi(fields[6]);
    row.state_r = strtod(fields[7], &endptr);
    row.FIFO_queue_buffer = atoi(fields[8]);
    row.tau = strtol(fields[9], NULL, 10);

    // Set start and end indices
    row.start_index = *start_index;
    row.end_index = *start_index + row.number - 1;

    // Update the start index for the next configuration
    *start_index = row.end_index + 1;

    row.loss = 0.0;
    return row;
}

/**
 * @brief Creates a csv_configuration from a CSV file.
 *
 * This function reads the CSV file, counts the rows and columns,
 * and populates the csv_configuration structure with row configurations.
 *
 * @param filename The name of the CSV file to read.
 * @param config A pointer to a common_configuration structure to copy settings from.
 * @return A populated csv_configuration structure.
 */
csv_configuration create_csv_configuration(const char *filename, common_configuration *config)
{
    csv_configuration csv_config;

    // Count rows and columns in the CSV file
    csv_config.kind_number = count_csv_rows(filename) - 1;
    csv_config.fields_number = count_csv_columns(filename);

    // Allocate memory for the rows
    csv_config.rows = (row_configuration *)malloc(csv_config.kind_number * sizeof(row_configuration));

    int tenant_number = 0;
    int start_index = 0;
    for (int i = 1; i < csv_config.kind_number + 1; i++)
    {
        // Read and split the CSV row into fields
        char **fields = split_csv_row(read_csv_row_by_index(filename, i), csv_config.fields_number);
        *(csv_config.rows + i - 1) = create_row_configuration(fields, &start_index);
        tenant_number += (csv_config.rows + i - 1)->number;
        // show_row_configuration(row);
        free(fields); // Free the array of fields
    }

    csv_config.tenant_number = tenant_number;
    config->tenant_number = tenant_number;

    csv_config.config = *config;

    csv_config.average_loss = 0.0;
    csv_config.compliant_loss = 0.0;
    csv_config.non_compliant_loss = 0.0;
    csv_config.uniform_loss = 0.0;
    csv_config.burst_loss = 0.0;

    return csv_config;
}

/**
 * @brief Frees the memory allocated for a csv_configuration structure.
 *
 * @param config The csv_configuration structure to free.
 */
void free_csv_configuration(csv_configuration config)
{
    free(config.rows); // Free the rows array
}

/**
 * @brief Displays the contents of a row_configuration.
 *
 * This function prints the details of a row_configuration structure
 * in a formatted manner, indicating whether the traffic mode is uniform
 * or burst.
 *
 * @param row The row_configuration structure to display.
 */
void show_row_configuration(const row_configuration row)
{
    printf(" index | traffic mode | mean | deviation | number | packet | real | state_r | FIFO |   tau  |  interval |    loss   |\n");
    if (row.traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION)
    {
        printf("   %d   |    uniform   |  %3d |    %3d    |  %3d   |  %3d   |  %3d | %.5f | %3d  |  %5ld | %3d ~ %3d | %2.7f |\n",
               row.index, row.mean, row.standard_deviation, row.number, row.packet_size, row.real_traffic, row.state_r,
               row.FIFO_queue_buffer, row.tau, row.start_index, row.end_index, row.loss);
    }
    else
    {
        printf("   %d   |     burst    |  %3d |    %3d    |  %3d   |  %3d   |  %3d | %.5f | %3d  |  %5ld | %3d ~ %3d | %2.7f |\n",
               row.index, row.mean, row.standard_deviation, row.number, row.packet_size, row.real_traffic, row.state_r,
               row.FIFO_queue_buffer, row.tau, row.start_index, row.end_index, row.loss);
    }
}

/**
 * @brief Displays the contents of a csv_configuration.
 *
 * This function prints the number of kinds and fields, followed by the details
 * of each row configuration, and finally the common configuration settings.
 *
 * @param config The csv_configuration structure to display.
 */
void show_csv_configuration(const csv_configuration config)
{
    printf("kind    : %d\n", config.kind_number);
    printf("field   : %d\n", config.fields_number);
    printf("number  : %d\n", config.tenant_number);
    print_equals_line();

    for (int i = 0; i < config.kind_number; i++)
    {
        show_row_configuration(config.rows[i]); // Display each row configuration
    }

    print_equals_line();

    printf("average loss       = %-3.17lf \%\n", config.average_loss);
    printf("compliant loss     = %-3.17lf \%\n", config.compliant_loss);
    printf("non-compliant loss = %-3.17lf \%\n", config.non_compliant_loss);
    printf("uniform loss       = %-3.17lf \%\n", config.uniform_loss);
    printf("burst loss         = %-3.17lf \%\n", config.burst_loss);

    show_configuration_common_configuration(config.config); // Display common settings
}

/**
 * @brief Tests the CSV configuration functions.
 *
 * This function creates a csv_configuration from a CSV file, displays its contents,
 * and then frees the allocated memory.
 *
 * @param filename The name of the CSV file to read.
 * @param config A pointer to a common_configuration structure to copy settings from.
 */
void test_csv_function(const char *filename, common_configuration *config)
{
    csv_configuration csv_config = create_csv_configuration(filename, config);

    // Display the CSV configuration
    show_csv_configuration(csv_config);

    // Free allocated memory for the CSV configuration
    free_csv_configuration(csv_config);

    // Uncomment for additional testing
    /*
    int row_number = count_csv_rows(filename);
    csv_config.kind_number = row_number - 1;
    printf("row    : %d\n", row_number);

    int column_number = count_csv_columns(filename);
    csv_config.fields_number = column_number;
    printf("column : %d\n", column_number);

    printf("%s\n", read_csv_row_by_index(filename, 1));
    char** fields = split_csv_row(read_csv_row_by_index(filename, 1), csv_config.fields_number);
    for (int i = 0; i < csv_config.fields_number; i++) {
        printf("%s\n", fields[i]);
    }

    int start_index = 0;
    for (int i = 1; i < row_number; i++) {
        char** fields = split_csv_row(read_csv_row_by_index(filename, i), csv_config.fields_number);
        row_configuration row = create_row_configuration(fields, &start_index);
        show_row_configuration(row);
        // Free fields after use
        for (int j = 0; j < csv_config.fields_number; j++) {
            free(fields[j]);
        }
        free(fields);
    }
    */
}
#endif