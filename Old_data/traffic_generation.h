/**
 * @file traffic_generation.h
 * @brief Defines a traffic generator structure and related constants.
 *
 * This header file declares the `traffic_generator` structure, which is used
 * for managing traffic generation parameters and associated state machines.
 *
 * The file includes the necessary header for the state machine functionality
 * and defines a macro for ensuring proper inclusion guards.
 */

#include "../include/state_machine.h"

#define TRAFFIC_GENERATION_H /**< Include guard to prevent multiple inclusions. */

#ifdef TRAFFIC_GENERATION_H

/**
 * @struct traffic_generator
 * @brief Traffic generation configuration and state management structure
 * @details This structure manages the configuration parameters and states for
 *          network traffic generation simulation. It includes timing parameters,
 *          probability settings, and state tracking for multiple traffic sources.
 *
 * The structure supports both compliant and non-compliant traffic generation
 * with separate probability controls.
 *
 * Memory layout:
 * @verbatim
 * +-----------------+
 * | step_size       |
 * +-----------------+
 * | grids_number    |
 * +-----------------+
 * | probabilities   |
 * +-----------------+
 * | states pointer  |---> [State Machine Array]
 * +-----------------+
 * @endverbatim
 */
typedef struct
{
    /** @name Timing Parameters */
    ///@{
    TIME_TYPE step_size; /**< Simulation time step increment */
    long grids_number;   /**< Total number of simulation time steps */
    ///@}

    /** @name Traffic Generation Probabilities */
    ///@{
    double generate_probability;              /**< Packet generation probability for compliant traffic */
    double generate_probability_noncompliant; /**< Packet generation probability for non-compliant traffic */
    ///@}

    /** @name State Management */
    ///@{
    state_machine *states; /**< Array of state machines for traffic generation */
    ///@}
} traffic_generator;

/**
 * @brief Validates traffic generator initialization results
 * @param[in] generator Initialized traffic generator to validate
 * @return bool true if valid, false otherwise
 */
static bool validateTrafficGenerator(const traffic_generator *generator)
{
    if (!generator)
    {
        return false;
    }

    /* Validate timing parameters */
    if (generator->step_size <= 0 || generator->grids_number <= 0)
    {
        return false;
    }

    /* Validate probabilities */
    if (generator->generate_probability < 0 ||
        generator->generate_probability > 1.0 ||
        generator->generate_probability_noncompliant < 0 ||
        generator->generate_probability_noncompliant > 1.0)
    {
        return false;
    }

    return true;
}

/**
 * @brief Helper function to calculate timing parameters
 * @param[in] input_rate Input packet rate
 * @param[in] packet_size Size of each packet
 * @return Calculated step size
 */
static TIME_TYPE calculateStepSize(uint64_t input_rate, uint32_t packet_size)
{
    double time_per_packet = (double)ONE_SECOND_IN_NS / input_rate;
    return (TIME_TYPE)(packet_size * time_per_packet);
}

/**
 * @brief Helper function to calculate generation probabilities
 * @param[in] mean Mean packet rate
 * @param[in] unit Base unit for calculations
 * @param[in] input_rate Input packet rate
 * @return Calculated generation probability
 */
static double calculateProbability(uint64_t mean, long unit, uint64_t input_rate)
{
    return (double)mean * unit / input_rate;
}

/**
 * @file traffic_generator_init.c
 * @brief Traffic generator initialization implementation
 */

/**
 * @brief Initializes a traffic generator with specified configuration
 * @details This function performs the following initialization steps:
 *          1. Calculates timing parameters (step size and grid number)
 *          2. Computes generation probabilities for both compliant and non-compliant traffic
 *          3. Initializes state machines for bursty traffic modes
 *
 * Timing calculations:
 * - Time per packet = ONE_SECOND_IN_NS / input_rate
 * - Step size = packet_size * time_per_packet
 * - Grid number = ONE_SECOND_IN_NS / step_size
 *
 * Probability calculations:
 * - Generate probability = (mean * unit) / input_rate
 * - Non-compliant probability = (noncompliant_mean * unit) / input_rate
 *
 * @param[in] config Configuration parameters for traffic generation
 * @return Initialized traffic_generator structure
 *
 * @note State machines are only initialized for bursty traffic modes
 * @warning Ensure config parameters are valid before calling this function
 *
 * @see configuration
 * @see traffic_generator
 * @see initialize_state_machines
 *
 * Example usage:
 * @code
 * configuration config = {
 *     .input_rate = 1000000,         // 1M packets per second
 *     .packet_size = 1500,           // 1500 bytes
 *     .mean = 800000,                // 800K compliant packets
 *     .noncompliant_mean = 200000,   // 200K non-compliant packets
 *     .traffic_mode = TRAFFIC_MODE_BURSTY_ALL,
 *     .unit = 1000000                // 1M unit
 * };
 * traffic_generator gen = initializeTrafficGenerator(config);
 * @endcode
 */
traffic_generator initializeTrafficGenerator(const configuration config)
{
    traffic_generator generate;

    /* Input validation */
    assert(config.input_rate > 0);
    assert(config.packet_size > 0);
    assert(config.mean >= 0);
    assert(config.noncompliant_mean >= 0);
    assert(config.unit > 0);

    /* Step 1: Calculate timing parameters */
    const double time_per_packet = (double)ONE_SECOND_IN_NS / config.input_rate;
    generate.step_size = (TIME_TYPE)(config.packet_size * time_per_packet);
    generate.grids_number = (long)((double)ONE_SECOND_IN_NS / generate.step_size);

    /* Step 2: Calculate generation probabilities */
    const long unit = config.unit;
    generate.generate_probability =
        (double)config.mean * unit / config.input_rate;
    generate.generate_probability_noncompliant =
        (double)config.noncompliant_mean * unit / config.input_rate;

    /* Step 3: Initialize state machines for bursty traffic modes */
    switch (config.traffic_mode)
    {
    case TRAFFIC_MODE_BURSTY_ALL:
    case TRAFFIC_MODE_BURSTY_COMPLIANT:
    case TRAFFIC_MODE_BURSTY_NONCOMPLIANT:
    case TRAFFIC_MODE_DIFFERENT_R:
        generate.states = initialize_state_machines(
            generate.generate_probability,
            generate.generate_probability_noncompliant,
            config);
        break;

    default:
        generate.states = NULL;
        break;
    }

    /* Validate initialization results */
    assert(generate.step_size > 0);
    assert(generate.grids_number > 0);
    assert(generate.generate_probability >= 0 &&
           generate.generate_probability <= 1.0);
    assert(generate.generate_probability_noncompliant >= 0 &&
           generate.generate_probability_noncompliant <= 1.0);

    return generate;
}

/**
 * @brief Helper function to calculate packet transmission time interval
 * @param[in] packet_size Packet size in bytes
 * @param[in] input_rate Packets per second
 * @return Time interval in nanoseconds
 */
static double calculate_time_interval(uint32_t packet_size, uint64_t input_rate)
{
    return (double)packet_size * (double)ONE_SECOND_IN_NS / (double)input_rate;
}

/**
 * @brief Helper function to calculate grids per second
 * @param[in] time_interval Time interval in nanoseconds
 * @return Number of grids per second
 */
static double calculate_grids_per_second(double time_interval)
{
    return (double)ONE_SECOND_IN_NS / time_interval;
}

/**
 * @brief Validates grid calculation parameters
 * @param[in] config Configuration structure
 * @return bool true if parameters are valid, false otherwise
 */
static bool validate_grid_parameters(const configuration *config)
{
    if (!config)
    {
        return false;
    }

    if (config->packet_size <= 0 ||
        config->input_rate <= 0 ||
        config->simulation_time <= 0)
    {
        return false;
    }

    return true;
}

/**
 * @brief Safe version of grid number calculation with error checking
 * @param[in] config Configuration structure
 * @param[out] result Calculated grid number
 * @return int 0 on success, error code on failure
 */
int obtain_grids_number_safe(const configuration *config, long *result)
{
    if (!config || !result)
    {
        return -EINVAL;
    }

    if (!validate_grid_parameters(config))
    {
        return -EINVAL;
    }

    const double time_interval = calculate_time_interval(
        config->packet_size,
        config->input_rate);

    if (time_interval <= 0)
    {
        return -ERANGE;
    }

    const double grids_per_second = calculate_grids_per_second(time_interval);
    const long total_grids = (long)(grids_per_second * config->simulation_time) + 1;

    if (total_grids <= 0)
    {
        return -ERANGE;
    }

    *result = total_grids;
    return 0;
}

/**
 * @brief Calculates the total number of simulation time grids
 * @details This function determines the total number of discrete time steps (grids)
 *          needed for the simulation based on the following formula:
 *
 * @verbatim
                                ONE_SECOND_IN_NS
 * time_interval = packet_size * ---------------
                                  input_rate
 *
 *                   ONE_SECOND_IN_NS
 * grids_per_second = ---------------
                      time_interval
 *
 * total_grids = grids_per_second * simulation_time + 1
 * @endverbatim
 *
 * The extra grid (+1) is added to ensure complete coverage of the simulation period.
 *
 * @param[in] config Configuration parameters including:
 *                   - packet_size: Size of each packet in bytes
 *                   - input_rate: Number of packets per second
 *                   - simulation_time: Total simulation duration in seconds
 *
 * @return Total number of simulation grids
 *
 * @pre config.packet_size > 0
 * @pre config.input_rate > 0
 * @pre config.simulation_time > 0
 *
 * @note The calculation uses double precision to maintain accuracy
 * @warning Ensure input parameters are within valid ranges to prevent overflow
 *
 * Example usage:
 * @code
 * configuration config = {
 *     .packet_size = 1500,      // 1500 bytes per packet
 *     .input_rate = 1000000,    // 1M packets per second
 *     .simulation_time = 10      // 10 seconds simulation
 * };
 * long grids = obtain_grids_number(config);
 * printf("Total simulation grids: %ld\n", grids);
 * @endcode
 */
long obtain_grids_number(const configuration config)
{
    /* Input validation */
    assert(config.packet_size > 0);
    assert(config.input_rate > 0);
    assert(config.simulation_time > 0);

    /* Calculate time interval for single packet transmission */
    const double time_interval = (double)config.packet_size *
                                 (double)ONE_SECOND_IN_NS /
                                 (double)config.input_rate;

    /* Calculate grids per second */
    const double grids_per_second = (double)ONE_SECOND_IN_NS / time_interval;

    /* Calculate total grids for the entire simulation period */
    const long total_grids = (long)(grids_per_second * config.simulation_time) + 1;

    /* Validate result */
    assert(total_grids > 0);

    return total_grids;
}

/**
 * @brief Displays the traffic generator parameters.
 *
 * This function prints the current parameters of the `traffic_generator` structure,
 * including the step size, number of grids, and packet generation probabilities.
 *
 * @param generate The `traffic_generator` structure containing the traffic generation parameters.
 */
void showTrafficGenerator(const traffic_generator generate)
{
    print_equals_line();
    printf("| step_size                    : %-lF\n", generate.step_size);                   /**< Print step size. */
    printf("| grids_number                 : %-ld\n", generate.grids_number);                /**< Print number of grids. */
    printf("| generate_probability         : %-f\n", generate.generate_probability);         /**< Print generation probability. */
    printf("| generate_probability_naughty : %-f\n", generate.generate_probability_naughty); /**< Print naughty packet generation probability. */
    print_equals_line();
}

/**
 * @brief Simulates a uniform distribution for packet acceptance.
 *
 * This function generates a random number between 0 and 1 using `drand48()`,
 * and compares it with the given `probability`. If the generated number is less
 * than or equal to the probability, the function returns `PACKET_LABEL_ACCEPT`;
 * otherwise, it returns `PACKET_LABEL_NO_PACKET`.
 *
 * @param probability The probability threshold for accepting a packet (between 0 and 1).
 * @return `PACKET_LABEL_ACCEPT` if the generated random number is less than or equal to `probability`,
 *         otherwise returns `PACKET_LABEL_NO_PACKET`.
 *
 * @note Ensure that `srand48(time(NULL));` is called once before using this function
 *       to properly seed the random number generator.
 */
int uniform_distribution(double probability)
{
    // srand((unsigned int)time(NULL) + random); /**< Random number generator seeding (commented out). */
    double factor = drand48(); /**< Generate a random number in the range [0, 1). */
    if (factor <= probability)
        return PACKET_LABEL_ACCEPT; /**< Accept the packet if the factor is within the probability. */
    else
        return PACKET_LABEL_NO_PACKET; /**< Reject the packet otherwise. */
}

/**
 * @brief Generates packet acceptance based on uniform distribution for traffic mode = TRAFFIC_MODE_INTERVAL.
 *
 * This function generates packets for a given number of tenants based on a uniform distribution.
 * Each tenant's packet acceptance is determined by a random probability.
 *
 * @param probability The probability for packet acceptance.
 * @param tenant_number The number of tenants for packet generation.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_uniform(double probability, int tenant_number)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number); /**< Allocate memory for packet array. */
    for (long index = 0; index < tenant_number; index++)
    {
        *(packets + index) = uniform_distribution(probability); /**< Determine packet acceptance for each tenant. */
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets with a "naughty" probability for traffic mode = TRAFFIC_MODE_NAUGHTY.
 *
 * This function generates packets for a given number of tenants based on their probability
 * of being "naughty" or regular, depending on whether the tenant index is within the "naughty" tenant range.
 *
 * @param generator The traffic generator containing the states and probabilities.
 * @param config The configuration containing the number of tenants and naughty tenant number.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_naughty(traffic_generator generator, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        // Naughty tenants use a different probability for packet generation.
        if (is_naughty_index(index, config))
            *(packets + index) = uniform_distribution(generator.generate_probability_naughty);
        else
            *(packets + index) = uniform_distribution(generator.generate_probability);
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets for all tenants based on a uniform "naughty" probability for traffic mode = TRAFFIC_MODE_ALL_NAUGHTY.
 *
 * This function generates packets for a given number of tenants based on a uniform "naughty" probability for all tenants.
 *
 * @param ratio_naughty The probability ratio for naughty packet acceptance.
 * @param tenant_number The number of tenants for packet generation.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_all_naughty(double ratio_naughty, int tenant_number)
{
    int *packets = (int *)malloc(sizeof(int) * tenant_number); /**< Allocate memory for packet array. */
    for (int index = 0; index < tenant_number; index++)
    {
        *(packets + index) = uniform_distribution(ratio_naughty); /**< Generate packets based on naughty probability. */
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets based on density and a normal distribution for traffic mode = TRAFFIC_MODE_DENSITY.
 *
 * This function generates packets for a given number of tenants based on the traffic density,
 * considering both mean and standard deviation values for packet acceptance.
 *
 * @param config The configuration containing mean, standard deviation, and other parameters.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_density(const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    // Calculate the packet generation probability based on density.
    double probability = (double)(config.mean + config.standard_deviation) * config.unit / config.input_rate;
    if (probability > 1.0)
        probability = 1.0; /**< Ensure probability does not exceed 1.0. */

    for (int index = 0; index < config.tenant_number; index++)
    {
        *(packets + index) = uniform_distribution(probability); /**< Generate packets based on calculated probability. */
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets for all tenants based on bursty traffic for traffic mode = TRAFFIC_MODE_BURSTY_ALL.
 *
 * This function generates packets based on bursty traffic mode. The state machine is used to determine whether
 * each tenant should create a packet or not.
 *
 * @param states The array of state machines for each tenant.
 * @param config The configuration containing the number of tenants.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_brusty_all(state_machine *states, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    change_states(states, config.tenant_number); /**< Update states for all tenants. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        if ((states + index)->state == STATE_CREATE_PACKET)
            *(packets + index) = PACKET_LABEL_ACCEPT; /**< Accept packet if state is STATE_CREATE_PACKET. */
        else
            *(packets + index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets based on bursty traffic with regular and naughty tenants for traffic mode = TRAFFIC_MODE_BURSTY_REGULAR.
 *
 * This function generates packets for bursty traffic mode, considering both regular and naughty tenants.
 * Regular tenants follow the state machine, while naughty tenants follow the "naughty" probability.
 *
 * @param generator The traffic generator containing the states and probabilities.
 * @param config The configuration containing the number of tenants and naughty tenant number.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_brusty_regular(traffic_generator generator, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    change_states(generator.states, config.tenant_number); /**< Update states for all tenants. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        if (is_naughty_index(index, config))
            *(packets + index) = uniform_distribution(generator.generate_probability_naughty); /**< Naughty tenants use naughty probability. */
        else
        {
            if ((generator.states + index)->state == STATE_CREATE_PACKET)
                *(packets + index) = PACKET_LABEL_ACCEPT; /**< Regular tenants accept packets if state is STATE_CREATE_PACKET. */
            else
                *(packets + index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
        }
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets based on bursty traffic with a mix of regular and naughty tenants for traffic mode = TRAFFIC_MODE_BURSTY_NAUGHTY.
 *
 * This function generates packets for bursty traffic mode, where naughty tenants have a separate packet acceptance probability.
 * The regular tenants follow the state machine for packet acceptance.
 *
 * @param generator The traffic generator containing the states and probabilities.
 * @param config The configuration containing the number of tenants and naughty tenant number.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_brusty_naughty(traffic_generator generator, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    change_states(generator.states, config.tenant_number); /**< Update states for all tenants. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        if (is_naughty_index(index, config))
        {
            if ((generator.states + index)->state == STATE_CREATE_PACKET)
                *(packets + index) = PACKET_LABEL_ACCEPT; /**< Accept packet if state is STATE_CREATE_PACKET for naughty tenants. */
            else
                *(packets + index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
        }
        else
        {
            *(packets + index) = uniform_distribution(generator.generate_probability); /**< Regular tenants follow uniform distribution. */
        }
    }
    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Generates packets for different traffic modes based on the provided configuration.
 *
 * This function selects the appropriate packet generation method based on the traffic mode
 * specified in the configuration. It handles several traffic modes such as uniform,
 * naughty, bursty, and density.
 *
 * @param generator The traffic generator containing state machines and generation probabilities.
 * @param config The configuration containing traffic mode and tenant details.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_configuration(traffic_generator generator, const configuration config)
{
    int *packets;

    // Select packet generation method based on the traffic mode.
    switch (config.traffic_mode)
    {
    case TRAFFIC_MODE_INTERVAL:
        packets = packet_generation_uniform(generator.generate_probability, config.tenant_number);
        break;

    case TRAFFIC_MODE_NAUGHTY:
        packets = packet_generation_naughty(generator, config);
        break;

    case TRAFFIC_MODE_ALL_NAUGHTY:
        packets = packet_generation_all_naughty(generator.generate_probability_naughty, config.tenant_number);
        break;

    case TRAFFIC_MODE_DENSITY:
        packets = packet_generation_density(config);
        break;

    case TRAFFIC_MODE_BURSTY_ALL:
        packets = packet_generation_brusty_all(generator.states, config);
        break;

    case TRAFFIC_MODE_BURSTY_REGULAR:
        packets = packet_generation_brusty_regular(generator, config);
        break;

    case TRAFFIC_MODE_BURSTY_NAUGHTY:
        packets = packet_generation_brusty_naughty(generator, config);
        break;
    case TRAFFIC_MODE_DIFFERENT_R:
        packets = packet_generation_brusty_all(generator.states, config);
        break;
    default:
        // Handle unexpected traffic modes
        printf(RED_ELOG "TRAFFIC MODE ERROR : CODE DOES NOT EXIST!\n" RESET);
        exit(EXIT_FAILURE);
    }

    return packets; /**< Return the generated packet array. */
}

/**
 * @brief Prints the generated packet information for each tenant and calculates the sum of accepted packets.
 *
 * This function prints the packet acceptance information for each tenant, showing whether a
 * packet was accepted or not. It also calculates and displays the total number of accepted packets
 * and the acceptance ratio.
 *
 * @param packets A pointer to the array of packet acceptance values for each tenant.
 * @param tenant_number The number of tenants for which the packet information is displayed.
 */
void print_packets(int *packets, long tenant_number)
{
    long sum = 0; /**< Variable to store the sum of accepted packets. */

    // Iterate over all tenants to print their packet acceptance information.
    for (long index = 0; index < tenant_number; index++)
    {
        if (*(packets + index) == PACKET_LABEL_ACCEPT)
            sum += 1; /**< Increment the sum if the packet is accepted. */

        printf("[%-10ld] : %-d\n", index, *(packets + index)); /**< Print the packet status for each tenant. */
    }

    // Print the sum of accepted packets and the acceptance ratio.
    printf("sum = %ld, length = %ld %f\n", sum, tenant_number, (double)sum / tenant_number);
}

#endif /* TRAFFIC_GENERATION_H */

// Test code
// traffic_generator generator = initializeTrafficGenerator(config);
// showTrafficGenerator(generator);

// for(int i=0;i<10;i++){
//     int* packets;
//     switch(config.traffic_mode){
//         case TRAFFIC_MODE_INTERVAL:
//             packets = packet_generation_uniform(generator.generate_probability, config.tenant_number);
//         break;

//         case TRAFFIC_MODE_NAUGHTY:
//             packets = packet_generation_naughty(generator, config);
//         break;

//         case TRAFFIC_MODE_ALL_NAUGHTY:
//             packets = packet_generation_all_naughty(generator.generate_probability_naughty, config.tenant_number);
//         break;

//         case TRAFFIC_MODE_DENSITY:
//             packets = packet_generation_density(config);
//         break;

//         case TRAFFIC_MODE_BURSTY_ALL:
//             packets =packet_generation_brusty_all(generator.states, config);
//         break;

//         case TRAFFIC_MODE_BURSTY_REGULAR:
//             packets = packet_generation_brusty_regular(generator, config);
//         break;

//         case TRAFFIC_MODE_BURSTY_NAUGHTY:
//             packets = packet_generation_brusty_naughty(generator, config);
//         break;

//         default:
//             printf(RED_ELOG"TRAFFIC MODE ERROR : CODE DOES NOT EXIST!\n"RESET);
//             exit(EXIT_FAILURE);
//     }
//     print_packets(packets, config.tenant_number);
// }printf("\n");

// for(int i=0;i<10;i++){
//     int* packets = packet_generation_configuration(generator, config);
//     print_packets(packets, config.tenant_number);
// }printf("\n");