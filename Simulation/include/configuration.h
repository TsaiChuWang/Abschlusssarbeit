
#include <string.h>

#define CONFIG_H

/**
 * @brief Enumeration defining the various traffic modes used for regulating tenant network behavior.
 */
#define TRAFFIC_MODE_INTERVAL 0       /**< All tenants follow the rules: traffic occurs within the specified interval, following a uniform distribution. */
#define TRAFFIC_MODE_GAUSSIAN 1       /**< All tenants follow the rules: traffic occurs within the specified interval, following a normal distribution. */
#define TRAFFIC_MODE_ALL_NAUGHTY 2    /**< All tenants violate the rules: the long-term mean traffic exceeds the specified mean (μ), following a uniform distribution. */
#define TRAFFIC_MODE_NAUGHTY 3        /**< Some tenants violate the rules, while others comply; traffic follows a uniform distribution. */
#define TRAFFIC_MODE_DENSITY 4        /**< All tenants transmit traffic at the upper bound of the allowed volume. */
#define TRAFFIC_MODE_BURSTY_ALL 5     /**< Some tenants violate the rules, while others comply; all traffic bursts are determined by a state machine. */
#define TRAFFIC_MODE_BURSTY_REGULAR 6 /**< Some tenants violate the rules, while others comply; regular tenants' traffic bursts are determined by a state machine. */
#define TRAFFIC_MODE_BURSTY_NAUGHTY 7 /**< Some tenants violate the rules, while others comply; naughty tenants' traffic bursts are determined by a state machine. */

#define NAUGHTY_MODE_BEFORE 0
#define NAUGHTY_MODE_AFTER 1
#define NAUGHTY_MODE_AVERAGE 2

/// @brief Define REDUCTION to recover the default configuration.
#ifdef REDUCTION

/**
 * @brief Parameters related to the basic simulation.
 */
#define INITIAL_CONFIGURATION_TENANT_NUMBER 100        /**< Number of tenants in the initial configuration. */
#define INITIAL_CONFIGURATION_SIMULATION_TIME 5.0      /**< Total simulation time in the initial configuration. */
#define INITIAL_CONFIGURATION_ERROR 0.001              /**< Error tolerance in the initial configuration. */
#define INITIAL_CONFIGURATION_DATA_PATH "../data/main" /**< Path for unit data storage. */
#define INITIAL_CONFIGURATION_UNIT 1048576             /**< Unit definition for the simulation. */

/**
 * @brief Parameters related to traffic generation.
 */
#define INITIAL_CONFIGURATION_INPUT_RATE (long)167772160 /**< Input rate for traffic generation. 167772160*/
#define INITIAL_CONFIGURATION_TRAFFIC_MODE 0             /**< Traffic mode setting. */
#define INITIAL_CONFIGURATION_MEAN 120                   /**< Mean value for traffic generation. */
#define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40      /**< Standard deviation for traffic generation. */
#define INITIAL_CONFIGURATION_PACKET_SIZE 512            /**< Packet size in bytes. */

#define INITIAL_CONFIGURATION_NAUGHTY_MEAN 150              /**< Mean traffic for 'naughty' tenants. */
#define INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION 10 /**< Standard deviation for 'naughty' tenants. */
#define INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER 50      /**< Number of 'naughty' tenants. */
#define INITIAL_CONFIGURATION_STATE_R (float)0.9375         /**< State parameter for traffic generation. */
#define INITIAL_CONFIGURATION_NAUGHTY_MODE 0
/**
 * @brief Parameters related to thresholds.
 * These values cannot be obtained through formulaic calculations.
 */
#define INITIAL_CONFIGURATION_UPPER_LINK_BUFFER 1   /**< Upper link buffer size. */
#define INITIAL_CONFIGURATION_BUCKET_DEPTH 916      /**< Depth of the bucket buffer. */
#define INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER 100 /**< (Missing value) Buffer size for link queue. */

// modifiy the specified path configuration file to the default configuration
void reduction_inif_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Unable to open file  \"%s\" for writing\n", filename);
        perror("Unable to open file for writing");
        return;
    }

    fprintf(file, "[simulation]\n");
    fprintf(file, "tenant_number = %d\n", INITIAL_CONFIGURATION_TENANT_NUMBER);
    fprintf(file, "simulation_time = %lf\n", INITIAL_CONFIGURATION_SIMULATION_TIME);
    fprintf(file, "error = %f\n", INITIAL_CONFIGURATION_ERROR);
    fprintf(file, "data_path = %s\n", INITIAL_CONFIGURATION_DATA_PATH);
    fprintf(file, "unit = %ld\n", INITIAL_CONFIGURATION_UNIT);

    // Write the [traffic] section
    fprintf(file, "[traffic]\n");
    fprintf(file, "input_rate = %ld\n", INITIAL_CONFIGURATION_INPUT_RATE);
    fprintf(file, "traffic_mode = %d\n", INITIAL_CONFIGURATION_TRAFFIC_MODE);
    fprintf(file, "mean = %d\n", INITIAL_CONFIGURATION_MEAN);
    fprintf(file, "standard_deviation = %d\n", INITIAL_CONFIGURATION_STANDARD_DEVIATION);
    fprintf(file, "packet_size = %d\n", INITIAL_CONFIGURATION_PACKET_SIZE);

    fprintf(file, "naughty_mean = %d\n", INITIAL_CONFIGURATION_NAUGHTY_MEAN);
    fprintf(file, "naughty_standard_deviation = %d\n", INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION);
    fprintf(file, "naughty_tenant_number = %d\n", INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER);
    fprintf(file, "naughty_mode = %f\n", INITIAL_CONFIGURATION_NAUGHTY_MODE);
    fprintf(file, "state_r = %f\n", INITIAL_CONFIGURATION_STATE_R);

    // Write the [threshold] section
    fprintf(file, "[threshold]\n");
    fprintf(file, "upper_queue_buffer = %ld\n", INITIAL_CONFIGURATION_UPPER_LINK_BUFFER);
    fprintf(file, "tau = %ld\n", INITIAL_CONFIGURATION_BUCKET_DEPTH);
    fprintf(file, "link_queue_buffer = %ld\n", INITIAL_CONFIGURATION_LINK_QUEUE_BUFFER);

    // Close the file
    fclose(file);
}
#endif // REDUCTION

#ifdef CONFIG_H

// Struct defining the configuration items for simulation
typedef struct
{
    // Simulation settings
    int tenant_number;         // Number of tenants in the simulation
    TIME_TYPE simulation_time; // Simulation time
    double error;              // Simulation error margin
    char *data_path;           // Path to data storage or files
    long unit;                 // Unit of measurement (e.g., Mbps, Gbps)

    // Traffic settings
    long input_rate;        // Input traffic rate
    int traffic_mode;       // Traffic mode (e.g., constant, Gaussian)
    int mean;               // Mean value for traffic characteristics
    int standard_deviation; // Standard deviation for traffic distribution
    int packet_size;        // Size of traffic packets

    // Naughty traffic settings (non-standard or outlier traffic)
    int naughty_mean;               // Mean value for naughty traffic
    int naughty_standard_deviation; // Standard deviation for naughty traffic
    int naughty_tenant_number;      // Number of tenants generating naughty traffic
    int naughty_mode;

    // Threshold and flow control settings
    double state_r; // State variable for traffic control

    long upper_queue_buffer; // Upper queue buffer size
    long tau;                // Time constant or threshold value
    int link_queue_buffer;   // Link queue buffer size
} configuration;

/**
 * @brief Handler function to populate configuration structure from key-value pairs in the INI file.
 *
 * @param config Pointer to the configuration structure.
 * @param section Section name in the configuration file.
 * @param name Key name in the section.
 * @param value Value corresponding to the key.
 * @return Status of the operation (SUCCESS or FAILURE).
 */
static int handler(void *config, const char *section, const char *name, const char *value)
{
    configuration *pconfig = (configuration *)config;

#define MATCH(section_, name_) (strcmp(section, section_) == 0 && strcmp(name, name_) == 0)

    if (MATCH("simulation", "tenant_number"))
        pconfig->tenant_number = atoi(value);
    else if (MATCH("simulation", "simulation_time"))
        pconfig->simulation_time = atof(value);
    else if (MATCH("simulation", "error"))
        pconfig->error = atof(value);
    else if (MATCH("simulation", "data_path"))
    {
        char *temp_str = malloc(strlen(value) + 1);
        strcpy(temp_str, value);
        pconfig->data_path = temp_str;
    }
    else if (MATCH("simulation", "unit"))
        pconfig->unit = strtol(value, NULL, 10);

    else if (MATCH("traffic", "input_rate"))
        pconfig->input_rate = strtol(value, NULL, 10);
    else if (MATCH("traffic", "traffic_mode"))
        pconfig->traffic_mode = atoi(value);
    else if (MATCH("traffic", "mean"))
        pconfig->mean = atoi(value);
    else if (MATCH("traffic", "standard_deviation"))
        pconfig->standard_deviation = atoi(value);
    else if (MATCH("traffic", "packet_size"))
        pconfig->packet_size = atoi(value);

    else if (MATCH("traffic", "naughty_mean"))
        pconfig->naughty_mean = atoi(value);
    else if (MATCH("traffic", "naughty_standard_deviation"))
        pconfig->naughty_standard_deviation = atoi(value);
    else if (MATCH("traffic", "naughty_tenant_number"))
        pconfig->naughty_tenant_number = atoi(value);
    else if (MATCH("traffic", "naughty_mode"))
        pconfig->naughty_mode = atoi(value);
    else if (MATCH("traffic", "state_r"))
        pconfig->state_r = (double)atof(value);

    else if (MATCH("threshold", "upper_queue_buffer"))
        pconfig->upper_queue_buffer = strtol(value, NULL, 10);
    else if (MATCH("threshold", "tau"))
        pconfig->tau = strtol(value, NULL, 10);
    else if (MATCH("threshold", "link_queue_buffer"))
        pconfig->link_queue_buffer = atoi(value);
    return SUCCESS;
}

/**
 * @brief Obtain the system capacity from a file.
 *
 * @return The system capacity in double, or FAILED on error.
 */
double obtain_capacity()
{
    FILE *file;
    double capacity;

    file = fopen(CAPACITY_DATA_PATH, "r");
    if (file == NULL)
    {
        printf("Cannot read file %s\n", CAPACITY_DATA_PATH);
        perror("Error opening file " CAPACITY_DATA_PATH);
        return FAILED;
    }

    if (fscanf(file, "%lf", &capacity) != 1)
    {
        printf("Error reading capacity from file %s\n", CAPACITY_DATA_PATH);
        perror("Error reading capacity from file " CAPACITY_DATA_PATH);
        fclose(file);
        return FAILED;
    }

    fclose(file);
    return capacity;
}

/**
 * @brief Modify the INI file with the current configuration.
 *
 * @param filename Path to the INI file.
 * @param config Pointer to the configuration structure to be written to the file.
 */
void modify_ini_file(const char *filename, configuration *config)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Unable to open file  \"%s\" for writing\n", filename);
        perror("Unable to open file for writing");
        return;
    }

    fprintf(file, "[simulation]\n");
    fprintf(file, "tenant_number = %d\n", config->tenant_number);
    fprintf(file, "simulation_time = %lf\n", config->simulation_time);
    fprintf(file, "error = %f\n", config->error);
    fprintf(file, "data_path = %s\n", config->data_path);
    fprintf(file, "unit = %d\n", config->unit);

    // Write the [traffic] section
    fprintf(file, "[traffic]\n");
    fprintf(file, "input_rate = %d\n", config->input_rate);
    fprintf(file, "traffic_mode = %d\n", config->traffic_mode);
    fprintf(file, "mean = %d\n", config->mean);
    fprintf(file, "standard_deviation = %d\n", config->standard_deviation);
    fprintf(file, "packet_size = %d\n", config->packet_size);

    fprintf(file, "naughty_mean = %d\n", config->naughty_mean);
    fprintf(file, "naughty_standard_deviation = %d\n", config->naughty_standard_deviation);
    fprintf(file, "naughty_tenant_number = %d\n", config->naughty_tenant_number);
    fprintf(file, "naughty_mode = %d\n", config->naughty_mode);
    fprintf(file, "state_r = %f\n", config->state_r);

    // Write the [threshold] section
    fprintf(file, "[threshold]\n");
    fprintf(file, "upper_queue_buffer = %ld\n", config->upper_queue_buffer);
    fprintf(file, "tau = %ld\n", config->tau);
    fprintf(file, "link_queue_buffer = %d\n", config->link_queue_buffer);

    // Close the file
    fclose(file);
}

/**
 * @brief Show the configuration values.
 *
 * @param config Configuration structure to be printed.
 */
void show_configuration(const configuration config)
{
    printf("- Simulation :\n");
    printf("| tenant number              : %-d\n", config.tenant_number);
    printf("| simulation time            : %-lf\n", config.simulation_time);
    printf("| error                      : %-f\n", config.error);
    printf("| data path                  : %-s\n", config.data_path);

    switch (config.unit)
    {
    case GBPS:
        printf("| unit                       : Gbps\n");
        break;
    case MBPS:
        printf("| unit                       : Mbps\n");
        break;
    case KBPS:
        printf("| unit                       : kbps\n");
        break;
    default:
        printf("| unit                       : Mbps\n");
    }

    printf("- Traffic :\n");
    printf("| input rate                 : %-ld\n", config.input_rate);
    printf("| traffic mode               : %-d\n", config.traffic_mode);
    printf("| mean                       : %-d\n", config.mean);
    printf("| standard deviation         : %-d\n", config.standard_deviation);
    printf("| packet size                : %-d\n", config.packet_size);

    if (config.traffic_mode > TRAFFIC_MODE_GAUSSIAN)
    {
        printf("| naughty mean               : %-d\n", config.naughty_mean);
        printf("| naughty standard deviation : %-d\n", config.naughty_standard_deviation);
        printf("| naughty tenant number      : %-d\n", config.naughty_tenant_number);
        printf("| naughty mode               : %-d\n", config.naughty_mode);
    }

    if (config.traffic_mode > TRAFFIC_MODE_DENSITY)
        printf("| state r                    : %-f\n", config.state_r);

    printf("- Threshold :\n");
    printf("| upper_queue_buffer         : %-ld\n", config.upper_queue_buffer);
    printf("| tau                        : %-ld\n", config.tau);
    printf("| link_queue_buffer          : %-d\n", config.link_queue_buffer);
}
#endif