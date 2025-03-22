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
 * @brief Structure to manage traffic generation parameters and state machines.
 *
 * This structure holds the configuration for the traffic generator, including
 * the step size, the number of grids, and the probabilities for packet generation.
 * It also contains a pointer to an array of `state_machine` structures, which are 
 * used to track the state of the traffic generation process.
 */
typedef struct
{
    TIME_TYPE step_size; /**< The time step size for the traffic generation process. */
    long grids_number;   /**< The total number of grids or simulation steps. */

    double generate_probability;         /**< Probability for generating a packet. */
    double generate_probability_naughty; /**< Probability for generating a "naughty" packet. */

    state_machine* states; /**< Pointer to an array of state machines. */
} traffic_generator;

/**
 * @brief Initializes a traffic generator with configuration parameters.
 *
 * This function computes the necessary traffic generation parameters, such as 
 * the step size, grid number, and generation probabilities. It also initializes 
 * the state machines based on the configuration, depending on the traffic mode.
 *
 * @param config Configuration structure containing the parameters for traffic generation.
 * @return A `traffic_generator` structure initialized with the provided configuration.
 */
traffic_generator initializeTrafficGenerator(const configuration config)
{
    traffic_generator generate;

    double temp = 0;
    temp = (double)ONE_SECOND_IN_NS / config.input_rate; /**< Calculate time per packet. */
    generate.step_size = (TIME_TYPE)(config.packet_size * temp); /**< Calculate the step size. */
    generate.grids_number = (long)((double)ONE_SECOND_IN_NS / generate.step_size); /**< Calculate the number of grids. */

    long unit = config.unit;
    generate.generate_probability = (double)config.mean * unit / config.input_rate; /**< Calculate the generation probability. */
    generate.generate_probability_naughty = (double)config.naughty_mean * unit / config.input_rate; /**< Calculate the naughty packet generation probability. */

    /** 
     * Initialize state machines based on the traffic mode.
     * States are only initialized for bursty traffic modes.
     */
    if (config.traffic_mode == TRAFFIC_MODE_BURSTY_ALL || config.traffic_mode == TRAFFIC_MODE_BURSTY_REGULAR || config.traffic_mode == TRAFFIC_MODE_BURSTY_NAUGHTY) {
        generate.states = initialize_state_machines(config.state_r, generate.generate_probability, generate.generate_probability_naughty, config.tenant_number, config.naughty_tenant_number);
    } else {
        generate.states = NULL; /**< No state machines are created for non-bursty traffic modes. */
    }

    return generate;
}

/**
 * @brief Calculate the total number of grids required for the simulation.
 * @details This function calculates the number of grids based on the time interval to transmit a packet 
 *          and the total simulation time. The number of grids is computed by dividing one second 
 *          by the packet transmission time, then multiplying by the simulation time.
 * 
 * @param config The configuration structure containing the packet size, input rate, and simulation time.
 * @return long The total number of grids for the simulation.
 */
long obtain_grids_number(const configuration config)
{
    double time_interval = (double)config.packet_size*(double)ONE_SECOND_IN_NS/(double)config.input_rate; // Time interval that transmit a packet
    long grids_number = (long)((double)ONE_SECOND_IN_NS/time_interval);
    return  grids_number*config.simulation_time+1;
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
    printf("| step_size                    : %-lF\n", generate.step_size); /**< Print step size. */
    printf("| grids_number                 : %-ld\n", generate.grids_number); /**< Print number of grids. */
    printf("| generate_probability         : %-f\n", generate.generate_probability); /**< Print generation probability. */
    printf("| generate_probability_naughty : %-f\n", generate.generate_probability_naughty); /**< Print naughty packet generation probability. */
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
        if (index >= config.naughty_tenant_number)
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
    if(probability > 1.0)
        probability = 1.0; /**< Ensure probability does not exceed 1.0. */

    for (int index = 0; index < config.tenant_number; index++)
    {
        *(packets+index) = uniform_distribution(probability); /**< Generate packets based on calculated probability. */
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
int *packet_generation_brusty_all(state_machine* states, const configuration config)
{
    int *packets = (int *)malloc(sizeof(int) * config.tenant_number); /**< Allocate memory for packet array. */

    change_states(states, config.tenant_number); /**< Update states for all tenants. */
    for (int index = 0; index < config.tenant_number; index++)
    {
        if((states+index)->state == STATE_CREATE_PACKET)
            *(packets+index) = PACKET_LABEL_ACCEPT; /**< Accept packet if state is STATE_CREATE_PACKET. */
        else 
            *(packets+index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
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
        if (index >= config.naughty_tenant_number)
            *(packets + index) = uniform_distribution(generator.generate_probability_naughty); /**< Naughty tenants use naughty probability. */
        else
        {
            if((generator.states+index)->state == STATE_CREATE_PACKET)
                *(packets+index) = PACKET_LABEL_ACCEPT; /**< Regular tenants accept packets if state is STATE_CREATE_PACKET. */
            else 
                *(packets+index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
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
        if (index >= config.naughty_tenant_number)
        {
            if((generator.states+index)->state == STATE_CREATE_PACKET)
                *(packets+index) = PACKET_LABEL_ACCEPT; /**< Accept packet if state is STATE_CREATE_PACKET for naughty tenants. */
            else 
                *(packets+index) = PACKET_LABEL_NO_PACKET; /**< Otherwise, reject packet. */
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
int* packet_generation_configuration(traffic_generator generator, const configuration config)
{
    int* packets;
    
    // Select packet generation method based on the traffic mode.
    switch(config.traffic_mode){
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

        default:
            // Handle unexpected traffic modes
            printf(RED_ELOG"TRAFFIC MODE ERROR : CODE DOES NOT EXIST!\n"RESET);
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