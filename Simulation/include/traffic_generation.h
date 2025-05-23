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
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
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
 * @brief Frees the resources allocated for the traffic generator.
 *
 * This function checks if the traffic generator pointer is NULL. If it is not,
 * it frees the memory allocated for the states array and sets the pointer to NULL
 * to avoid dangling references.
 *
 * @param generate Pointer to the traffic_generator structure to be freed.
 */
void freeTrafficGenerator(traffic_generator *generate)
{
    if (generate == NULL) ///< Check if the traffic generator pointer is NULL.
        return;           ///< Exit the function if there is nothing to free.

    if (generate->states != NULL) ///< Check if the states array is allocated.
    {
        free(generate->states);  ///< Free the allocated memory for states.
        generate->states = NULL; ///< Set the pointer to NULL to avoid dangling reference.
    }
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
 * @brief Displays the traffic generator parameters in a formatted table
 * @details Prints a formatted table showing the current traffic generator parameters:
 *          - step_size: Time interval between packet transmissions
 *          - grids_number: Total number of time grids in simulation
 *          - generate_probability: Probability for compliant packet generation
 *          - generate_probability_noncompliant: Probability for non-compliant packet generation
 *
 * Example output:
 * @verbatim
 * ========================================================
 * | step_size                    : 1.234567e-06
 * | grids_number                 : 1000000
 * | generate_probability         : 0.800000
 * | generate_probability_noncompliant : 0.200000
 * ========================================================
 * @endverbatim
 *
 * @param[in] generate Traffic generator structure containing simulation parameters
 *
 * @note Output is directed to stdout using printf
 * @see traffic_generator
 * @see print_equals_line
 */
void showTrafficGenerator(const traffic_generator generate)
{
    /* Print top border line */
    print_equals_line();

    /* Display timing parameters */
    printf("| step_size                         : %-lF\n", generate.step_size);
    printf("| grids_number                      : %-ld\n", generate.grids_number);

    /* Display probability parameters */
    printf("| generate_probability              : %-f\n", generate.generate_probability);
    printf("| generate_probability_noncompliant : %-f\n", generate.generate_probability_noncompliant);

    /* Print bottom border line */
    print_equals_line();

    /* Ensure output is immediately visible */
    fflush(stdout);
}

/**
 * @brief Simulates packet acceptance using uniform distribution
 * @details Generates a random number using uniform distribution to determine
 *          whether a packet should be accepted based on given probability.
 *          Uses drand48() to generate random numbers in [0,1) range.
 *
 * Algorithm:
 * @verbatim
 * 1. Generate random number r in [0,1)
 * 2. If r <= probability: Accept packet
 * 3. Otherwise: No packet
 * @endverbatim
 *
 * @param[in] probability Acceptance probability threshold [0,1]
 * @return int PACKET_LABEL_ACCEPT if packet accepted,
 *             PACKET_LABEL_NO_PACKET if rejected
 *
 * @pre Probability must be in range [0,1]
 * @pre Random number generator must be seeded using srand48(time(NULL))
 *
 * @note Thread safety: drand48() is not thread-safe
 * @warning Ensure proper seeding before first call
 */
int uniform_distribution(double probability)
{
    /* Parameter validation */
    assert(probability >= 0.0 && probability <= 1.0);

    /* Generate random number in [0,1) range */
    const double random_factor = drand48();

    /* Compare with probability threshold */
    return (random_factor <= probability)
               ? PACKET_LABEL_ACCEPT
               : PACKET_LABEL_NO_PACKET;
}

/**
 * @brief Generates packet acceptance decisions for multiple tenants
 * @details Uses uniform distribution to determine packet acceptance for each tenant
 *          in TRAFFIC_MODE_UNIFORM mode. Each tenant's packet acceptance is
 *          independently determined based on the given probability.
 *
 * Algorithm:
 * @verbatim
 * 1. Allocate array for tenant decisions
 * 2. For each tenant:
 *    - Generate uniform random number
 *    - Compare with probability threshold
 *    - Store accept/reject decision
 * @endverbatim
 *
 * @param[in] probability Acceptance probability threshold [0,1]
 * @param[in] tenant_number Number of tenants to generate decisions for
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails
 *
 * @warning Caller must free the returned array
 * @note Thread safety: Uses non-thread-safe uniform_distribution()
 *
 * Example usage:
 * @code
 * int tenant_count = 5;
 * double prob = 0.7;
 * int *results = packet_generation_uniform(prob, tenant_count);
 * if (results) {
 *     // Process results
 *     free(results);
 * }
 * @endcode
 */
int *packet_generation_uniform(double probability, int tenant_number)
{
    /* Validate input parameters */
    if (probability < 0.0 || probability > 1.0 || tenant_number <= 0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Generate packet decisions for each tenant */
    for (int i = 0; i < tenant_number; i++)
    {
        packets[i] = uniform_distribution(probability);
    }

    return packets;
}

/**
 * @brief Generates packets with different probabilities for compliant and noncompliant tenants
 * @details Implements TRAFFIC_MODE_NONCOMPLIANT_UNIFORM behavior where certain tenants have
 *          different packet generation probabilities. Noncompliant tenants typically
 *          have higher packet generation rates to simulate aggressive behavior.
 *
 * Algorithm:
 * @verbatim
 * 1. Allocate array for tenant decisions
 * 2. For each tenant:
 *    - Check if tenant is in noncompliant range
 *    - Use appropriate probability (normal/noncompliant)
 *    - Generate and store packet decision
 * @endverbatim
 *
 * @param[in] generator Traffic generator state containing probabilities
 * @param[in] config System configuration with tenant information
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails
 *
 * @pre generator must be properly initialized
 * @pre config must contain valid tenant configuration
 * @warning Caller must free the returned array
 */
int *packet_generation_noncompliant(const traffic_generator generator,
                                    const configuration config)
{
    /* Validate input parameters */
    if (config.tenant_number <= 0 ||
        config.noncompliant_tenant_number < 0 ||
        config.noncompliant_tenant_number > config.tenant_number ||
        generator.generate_probability < 0.0 ||
        generator.generate_probability > 1.0 ||
        generator.generate_probability_noncompliant < 0.0 ||
        generator.generate_probability_noncompliant > 1.0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * config.tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Generate packet decisions for each tenant */
    for (int i = 0; i < config.tenant_number; i++)
    {
        double prob = is_noncompliant_index(i, config)
                          ? generator.generate_probability_noncompliant
                          : generator.generate_probability;
        packets[i] = uniform_distribution(prob);
    }

    return packets;
}

/**
 * @brief Generates packets with uniform noncompliant probability for all tenants
 * @details Implements TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM behavior where all tenants use
 *          the same noncompliant probability for packet generation. This simulates
 *          a scenario where all tenants exhibit aggressive behavior.
 *
 * Algorithm:
 * @verbatim
 * 1. Validate input parameters
 * 2. Allocate memory for decisions
 * 3. For each tenant:
 *    - Generate packet decision using noncompliant probability
 * 4. Return array of decisions
 * @endverbatim
 *
 * @param[in] ratio_noncompliant Probability of packet acceptance [0,1]
 * @param[in] tenant_number Number of tenants to generate for
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails or parameters are invalid
 *
 * @pre ratio_noncompliant must be in range [0,1]
 * @pre tenant_number must be positive
 * @warning Caller must free the returned array
 */
int *packet_generation_all_noncompliant(double ratio_noncompliant,
                                        int tenant_number)
{
    /* Validate input parameters */
    if (ratio_noncompliant < 0.0 || ratio_noncompliant > 1.0 ||
        tenant_number <= 0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Generate packet decisions for all tenants */
    for (int i = 0; i < tenant_number; i++)
    {
        packets[i] = uniform_distribution(ratio_noncompliant);
    }

    return packets;
}

/**
 * @brief Generates packets based on traffic density parameters
 * @details Implements TRAFFIC_MODE_DENSITY behavior where packet generation
 *          probability is calculated from mean traffic density and variation.
 *          Uses normal distribution properties to determine acceptance probability.
 *
 * Algorithm:
 * @verbatim
 * 1. Validate input parameters
 * 2. Calculate acceptance probability:
 *    prob = (mean + std_dev) * unit / input_rate
 *    Clamp probability to [0,1]
 * 3. Generate packet decisions for each tenant
 * @endverbatim
 *
 * @param[in] config Configuration containing:
 *                   - mean: Average traffic density
 *                   - standard_deviation: Traffic variation
 *                   - unit: Base traffic unit
 *                   - input_rate: System input rate
 *                   - tenant_number: Number of tenants
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails or parameters are invalid
 *
 * @pre All config parameters must be non-negative
 * @pre input_rate must be positive
 * @warning Caller must free the returned array
 */
int *packet_generation_density(const configuration config)
{
    /* Validate input parameters */
    if (config.mean < 0.0 ||
        config.standard_deviation < 0.0 ||
        config.unit <= 0.0 ||
        config.input_rate <= 0.0 ||
        config.tenant_number <= 0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * config.tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Generate packet decisions */
    for (int i = 0; i < config.tenant_number; i++)
    {
        packets[i] = PACKET_LABEL_ACCEPT;
    }

    return packets;
}

/**
 * @brief Generates packets using bursty traffic patterns
 * @details Implements TRAFFIC_MODE_BURSTY_ALL behavior where packet generation
 *          is controlled by state machines to create realistic burst patterns.
 *          Each tenant has its own state machine that transitions between
 *          packet creation and idle states.
 *
 * Algorithm:
 * @verbatim
 * 1. Validate input parameters
 * 2. Update state machines for all tenants
 * 3. Generate packets based on current states:
 *    - STATE_CREATE_PACKET -> Accept
 *    - Other states -> No packet
 * @endverbatim
 *
 * @param[in,out] states Array of state machines for each tenant
 * @param[in] config Configuration containing tenant count and parameters
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails or parameters are invalid
 *
 * @pre states must be initialized for all tenants
 * @pre config.tenant_number must match states array size
 * @warning Caller must free the returned array
 */
int *packet_generation_bursty_all(state_machine *states,
                                  const configuration config)
{
    /* Validate input parameters */
    if (!states || config.tenant_number <= 0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * config.tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Update state machines */
    change_states(states, config.tenant_number);

    /* Generate packets based on states */
    for (int i = 0; i < config.tenant_number; i++)
    {
        packets[i] = (states[i].state == STATE_CREATE_PACKET)
                         ? PACKET_LABEL_ACCEPT
                         : PACKET_LABEL_NO_PACKET;
    }

    return packets;
}

/**
 * @brief Generates packets for mixed compliant/noncompliant tenant populations
 * @details Implements TRAFFIC_MODE_BURSTY_COMPLIANT behavior where:
 *          - Compliant tenants follow state machine transitions
 *          - Noncompliant tenants use probability-based generation
 *
 * Algorithm:
 * @verbatim
 * 1. Validate input parameters
 * 2. Update state machines for all tenants
 * 3. For each tenant:
 *    - If noncompliant: Use probability-based generation
 *    - If compliant: Use state machine decision
 * @endverbatim
 *
 * @param[in,out] generator Traffic generator containing:
 *                         - states: Array of state machines
 *                         - generate_probability_noncompliant: Probability for noncompliant tenants
 * @param[in] config Configuration containing:
 *                   - tenant_number: Total number of tenants
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if memory allocation fails or parameters are invalid
 *
 * @pre generator.states must be initialized
 * @pre config parameters must be valid
 * @warning Caller must free the returned array
 *
 * Example usage:
 * @code
 * configuration cfg = {
 *     .tenant_number = 100,
 *     .noncompliant_tenant_number = 20
 * };
 * traffic_generator gen = init_traffic_generator(cfg);
 * int *results = packet_generation_bursty_compliant(gen, cfg);
 * if (results) {
 *     // Process results
 *     free(results);
 * }
 * free_traffic_generator(&gen);
 * @endcode
 */
int *packet_generation_bursty_compliant(traffic_generator generator,
                                        const configuration config)
{
    /* Validate input parameters */
    if (!generator.states ||
        config.tenant_number <= 0 ||
        config.noncompliant_tenant_number < 0 ||
        config.noncompliant_tenant_number > config.tenant_number ||
        generator.generate_probability_noncompliant < 0.0 ||
        generator.generate_probability_noncompliant > 1.0)
    {
        errno = EINVAL;
        return NULL;
    }

    /* Allocate memory for packet decisions */
    int *packets = malloc(sizeof(int) * config.tenant_number);
    if (!packets)
    {
        errno = ENOMEM;
        return NULL;
    }

    /* Update state machines */
    change_states(generator.states, config.tenant_number);

    /* Generate packets for each tenant */
    for (int i = 0; i < config.tenant_number; i++)
    {
        if (is_noncompliant_index(i, config))
        {
            packets[i] = uniform_distribution(
                generator.generate_probability_noncompliant);
        }
        else
        {
            packets[i] = (generator.states[i].state == STATE_CREATE_PACKET)
                             ? PACKET_LABEL_ACCEPT
                             : PACKET_LABEL_NO_PACKET;
        }
    }

    return packets;
}

/**
 * @brief Generates packets based on bursty traffic with a mix of compliant and noncompliant tenants for traffic mode = TRAFFIC_MODE_BURSTY_NONCOMPLIANT.
 *
 * This function generates packets for bursty traffic mode, where noncompliant tenants have a separate packet acceptance probability.
 * The compliant tenants follow the state machine for packet acceptance.
 *
 * @param generator The traffic generator containing the states and probabilities.
 * @param config The configuration containing the number of tenants and noncompliant tenant number.
 * @return A dynamically allocated array of integers representing packet acceptance for each tenant.
 */
int *packet_generation_bursty_noncompliant(traffic_generator generator, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    change_states(generator.states, config.tenant_number); /**< Update states for all tenants. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        if (is_noncompliant_index(index, config))
        {
            if ((generator.states + index)->state == STATE_CREATE_PACKET)
                *(packets + index) = PACKET_LABEL_ACCEPT; /**< Accept packet if state is STATE_CREATE_PACKET for noncompliant tenants. */
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
 * @brief Generates packets based on configured traffic mode
 * @details Dispatches packet generation to specialized handlers based on:
 *          - Uniform distribution
 *          - Noncompliant behavior
 *          - Bursty patterns
 *          - Density-based distribution
 *
 * Supported Traffic Modes:
 * - TRAFFIC_MODE_UNIFORM: Basic uniform distribution
 * - TRAFFIC_MODE_NONCOMPLIANT_UNIFORM: Mixed compliant/noncompliant
 * - TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM: All noncompliant
 * - TRAFFIC_MODE_DENSITY: Density-based generation
 * - TRAFFIC_MODE_BURSTY_ALL: Bursty pattern for all
 * - TRAFFIC_MODE_BURSTY_COMPLIANT: Bursty with compliance
 * - TRAFFIC_MODE_BURSTY_NONCOMPLIANT: Bursty noncompliant
 * - TRAFFIC_MODE_DIFFERENT_R: Variable rate bursty
 *
 * @param[in] generator Traffic generator containing:
 *                     - states: State machines
 *                     - generate_probability: Base probability
 *                     - generate_probability_noncompliant: Noncompliant probability
 * @param[in] config Configuration containing:
 *                   - traffic_mode: Selected generation mode
 *                   - tenant_number: Number of tenants
 *                   - Other mode-specific parameters
 * @return int* Array of packet decisions (PACKET_LABEL_ACCEPT or PACKET_LABEL_NO_PACKET)
 *         NULL if error occurs
 *
 * @pre generator must be properly initialized
 * @pre config must contain valid parameters
 * @warning Caller must free the returned array
 *
 * Example usage:
 * @code
 * configuration cfg = {
 *     .traffic_mode = TRAFFIC_MODE_BURSTY_COMPLIANT,
 *     .tenant_number = 100
 * };
 * traffic_generator gen = init_traffic_generator(cfg);
 * int *results = packet_generation_configuration(gen, cfg);
 * if (results) {
 *     // Process results
 *     free(results);
 * }
 * free_traffic_generator(&gen);
 * @endcode
 */
int *packet_generation_configuration(traffic_generator generator,
                                     const configuration config)
{
    int *packets = NULL;

    /* Select and execute generation method */
    switch (config.traffic_mode)
    {
    case TRAFFIC_MODE_UNIFORM:
        packets = packet_generation_uniform(
            generator.generate_probability,
            config.tenant_number);
        break;

    case TRAFFIC_MODE_NONCOMPLIANT_UNIFORM:
        packets = packet_generation_noncompliant(
            generator,
            config);
        break;

    case TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM:
        packets = packet_generation_all_noncompliant(
            generator.generate_probability_noncompliant,
            config.tenant_number);
        break;

    case TRAFFIC_MODE_DENSITY:
        packets = packet_generation_density(config);
        break;

    case TRAFFIC_MODE_BURSTY_ALL:
        packets = packet_generation_bursty_all(
            generator.states,
            config);
        break;

    case TRAFFIC_MODE_BURSTY_COMPLIANT:
        packets = packet_generation_bursty_compliant(
            generator,
            config);
        break;

    case TRAFFIC_MODE_BURSTY_NONCOMPLIANT:
        packets = packet_generation_bursty_noncompliant(
            generator,
            config);
        break;

    case TRAFFIC_MODE_DIFFERENT_R:
        packets = packet_generation_bursty_all(
            generator.states,
            config);
        break;

    default:
        printf(RED_ELOG "Invalid traffic mode: %d", config.traffic_mode);
        errno = EINVAL;
        return NULL;
    }

    /* Check generation success */
    if (!packets)
    {
        printf(RED_ELOG "Packet generation failed for mode: %d", config.traffic_mode);
        return NULL;
    }

    return packets;
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
    if (!packets || tenant_number <= 0)
    {
        printf("Error: Invalid input parameters\n");
        return;
    }

    long sum = 0; /**< Variable to store the sum of accepted packets. */

    /* Print header */
    printf("\n=== Packet Generation Status ===\n");
    printf("Total Tenants: %ld\n", tenant_number);
    printf("%-12s | %s\n", "Tenant ID", "Status");
    printf("-------------+--------\n");

    /* Process and display packet information */
    for (long index = 0; index < tenant_number; index++)
    {
        if (*(packets + index) == PACKET_LABEL_ACCEPT)
            sum += 1; /**< Increment the sum if the packet is accepted. */

        printf("[%-10ld] : %2d\n", index, *(packets + index));
    }

    printf("\n=== Summary ===\n");
    printf("Accepted Packets: %ld\n", sum);
    printf("Total Tenants   : %ld\n", tenant_number);
    printf("Accept Ratio    : %.3f \%\n", (double)sum * 100.0 / tenant_number);
}

void test_traffic_generator_h(configuration config)
{
    traffic_generator generator;
    printf("===== initializeTrafficGenerator(uniform) =====\n");
    config.traffic_mode = TRAFFIC_MODE_UNIFORM;
    generator = initializeTrafficGenerator(config);
    showTrafficGenerator(generator);
    printf("===== obtain_grids_number =====\n");
    printf("grid numbers = %ld\n", obtain_grids_number(config));
    printf("===== initializeTrafficGenerator(burst) =====\n");
    config.traffic_mode = TRAFFIC_MODE_DIFFERENT_R;
    generator = initializeTrafficGenerator(config);
    showTrafficGenerator(generator);
    printf("===== uniform_distribution =====\n");
    printf("uniform_distribution : %d\n", uniform_distribution(0.5));
    printf("uniform_distribution : %d\n", uniform_distribution(0.5));
    printf("uniform_distribution : %d\n", uniform_distribution(0.5));
    int *packets;
    printf("===== TRAFFIC_MODE_UNIFORM =====\n");
    config.traffic_mode = TRAFFIC_MODE_UNIFORM;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
    printf("===== TRAFFIC_MODE_NONCOMPLIANT_UNIFORM =====\n");
    config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
    printf("===== TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM =====\n");
    config.traffic_mode = TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
    printf("===== TRAFFIC_MODE_DENSITY =====\n");
    config.traffic_mode = TRAFFIC_MODE_DENSITY;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
    printf("===== TRAFFIC_MODE_BURSTY_ALL =====\n");
    config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
    printf("===== TRAFFIC_MODE_DIFFERENT_R =====\n");
    config.traffic_mode = TRAFFIC_MODE_DIFFERENT_R;
    packets = packet_generation_configuration(generator, config);
    print_packets(packets, config.tenant_number);
}

/**
 * @brief Generates a shuffled array of integers from 0 to n-1.
 *
 * This function allocates memory for an array of integers, initializes it
 * with values from 0 to n-1, and then shuffles the array using the
 * Fisher-Yates algorithm.
 *
 * @param n The number of elements in the array.
 * @return int* Pointer to the shuffled array, or NULL if memory allocation fails.
 */
int *generateShuffledArray(int n)
{
    int *array = (int *)malloc((n) * sizeof(int)); ///< Allocate memory for the array.

    if (array == NULL) ///< Check if memory allocation was successful.
    {
        printf("Memory allocation failed\n"); ///< Print error message if allocation fails.
        return NULL;                          ///< Return NULL to indicate failure.
    }

    for (int i = 0; i < n; i++) ///< Initialize the array with values from 0 to n-1.
    {
        array[i] = i;
    }

    // Fisher-Yates shuffle algorithm
    for (int i = n - 1; i > 0; i--) ///< Iterate from the end of the array to the beginning.
    {
        int j = rand() % (i + 1); ///< Generate a random index.

        // Swap elements at indices i and j
        int temp = array[i]; ///< Store the current element in a temporary variable.
        array[i] = array[j]; ///< Replace the current element with the randomly selected element.
        array[j] = temp;     ///< Place the stored element in the randomly selected position.
    }

    return array; ///< Return the pointer to the shuffled array.
}

#endif /* TRAFFIC_GENERATION_H */