
/**
 * @file different_mode_traffic_generation_chart.c
 * @brief Implements traffic generation in different modes.
 * 
 * This file defines functionality for generating traffic patterns 
 * using different modes, utilizing a state machine for control.
 * 
 * Compilation:
 * @code
 * gcc ./different_mode_traffic_generation_chart.c inih/ini.c -o ../execution/different_mode_traffic_generation_chart -lm
 * @endcode
 * 
 * Execution:
 * @code
 * ../execution/different_mode_traffic_generation_chart
 * @endcode
 */

 #include "../include/general.h"
 #include "../include/state_machine.h"
 
 #define NAME "different_mode_traffic_generation_chart" /**< Program name identifier. */
 
/**
 * @brief Simulates a dice roll to determine the state.
 *
 * This function generates a random value and compares it with the given 
 * threshold to decide the resulting state.
 *
 * @param threshold The probability threshold for determining the state.
 * @return The resulting state of type `state_t`.
 */
state_t dice(double threshold);

int main(int argc, char *argv[])
{
    /**
     * @brief Entry point for the traffic generation program.
     *
     * This function initializes random seed, opens a CSV file for writing results,
     * and writes the header row to the file.
     *
     * @param argc Argument count.
     * @param argv Argument vector.
     * @return Returns 0 on successful execution.
     */

    srand48(time(NULL)); /**< Seed the random number generator. */

    FILE *file = fopen("../data/" NAME ".csv", "w+"); /**< Open a CSV file for writing. */
    if (!file)
    {
        perror("Error opening file"); /**< Print error if file opening fails. */
        exit(EXIT_FAILURE);
    }

    /** 
     * @brief Write CSV header row with state transition probabilities.
     */
    fprintf(file, "ru,nu,r0.875,n0.875,r0.9,n0.9,r0.9125,n0.9125,"
                  "r0.925,n0.925,r0.9375,n0.9375,r0.95,n0.95,"
                  "r0.9625,n0.9625,r0.975,n0.975,\n");

    /**
     * @brief Initializes multiple state machines with varying parameters.
     *
     * Each state machine is created using `initialize_state_machines()` 
     * with different values for the first parameter, while the other parameters 
     * remain constant.
     *
     * @details The function `initialize_state_machines()` takes five parameters:
     * - The first parameter varies (0.875 to 0.975).
     * - The remaining parameters (0.75, 0.875, 2, 1) are consistent across calls.
     *
     * The initialized state machines are stored as pointers.
     */

    state_machine* states_1 = initialize_state_machines(0.875, 0.75, 0.875, 2, 1);
    state_machine* states_2 = initialize_state_machines(0.9, 0.75, 0.875, 2, 1);
    state_machine* states_3 = initialize_state_machines(0.9125, 0.75, 0.875, 2, 1);
    state_machine* states_4 = initialize_state_machines(0.925, 0.75, 0.875, 2, 1);
    state_machine* states_5 = initialize_state_machines(0.9375, 0.75, 0.875, 2, 1);
    state_machine* states_6 = initialize_state_machines(0.95, 0.75, 0.875, 2, 1);
    state_machine* states_7 = initialize_state_machines(0.9625, 0.75, 0.875, 2, 1);
    state_machine* states_8 = initialize_state_machines(0.975, 0.75, 0.875, 2, 1);
 
    /**
     * @brief Simulates packet generation and logs results to a CSV file.
     *
     * This loop runs 327,680 iterations, simulating packet creation based on
     * probabilistic state transitions and updating a state machine.
     * 
     * - The `dice()` function determines whether a packet is created.
     * - The `change_states()` function updates each state machine's state.
     * - The results are logged to a CSV file.
     * - An integer array `count` tracks the count of packet creation events.
     */

    int* count = calloc(18, sizeof(int)); /**< Allocate memory to store packet counts. */

    for (int i = 0; i < 327680; i++) {
        /** 
          * @brief Determine packet creation using probability-based dice rolls.
          */
        if (dice(0.75) == STATE_CREATE_PACKET) {
            fprintf(file, "1,");
            *count += 1; /**< Increment counter for probability 0.75. */
        } else {
            fprintf(file, "0,");
        }
    
        if (dice(0.875) == STATE_CREATE_PACKET) {
            fprintf(file, "1,");
            *(count + 1) += 1; /**< Increment counter for probability 0.875. */
        } else {
            fprintf(file, "0,");
        }
    
        /** 
          * @brief Update all state machines.
          */
        change_states(states_1, 2);
        change_states(states_2, 2);
        change_states(states_3, 2);
        change_states(states_4, 2);
        change_states(states_5, 2);
        change_states(states_6, 2);
        change_states(states_7, 2);
        change_states(states_8, 2);
    
        /** 
          * @brief Log state machine results to CSV and update counters.
          */
        for (int j = 0; j < 8; j++) {
            if ((states_1 + j)->state == STATE_CREATE_PACKET) {
                fprintf(file, "1,");
                *(count + (j * 2) + 2) += 1;
            } else {
                fprintf(file, "0,");
            }
    
            if ((states_1 + j + 1)->state == STATE_CREATE_PACKET) {
                fprintf(file, "1,");
                *(count + (j * 2) + 3) += 1;
            } else {
                fprintf(file, "0,");
            }
        }
    
        fprintf(file, "\n"); /**< Newline to separate iterations. */
    }
 

    /**
     * @brief Prints the packet creation statistics.
     *
     * This loop iterates over the `count` array (size 18) and prints the count of 
     * packet creation events along with their occurrence probability as a percentage.
     *
     * @details The probability is calculated as:
     * \f[
     * P = \frac{\text{count}}{327680} \times 100
     * \f]
     *
     * where `count` is the number of times a packet was created in the simulation.
     */
    for (int j = 0; j < 18; j++) {
      printf("%d (%f %%)\n", *(count + j), (double) *(count + j) / 327680 * 100);
    }

    fclose(file); /**< Close the CSV file after logging. */
    free(count); /**< Free allocated memory for the integer array. */

    return EXIT_SUCCESS;
}

/**
 * @brief Simulates a probabilistic state transition.
 *
 * This function generates a random number between 0 and 1 using `drand48()`
 * and compares it to the provided `threshold` to determine the next state.
 *
 * @param threshold Probability threshold for transitioning to `STATE_CREATE_PACKET`.
 * @return `STATE_CREATE_PACKET` if the generated number is less than `threshold`,
 *         otherwise returns `STATE_NO_PACKET`.
 *
 * @note Ensure that `srand48(time(NULL));` is called once before using this function
 *       to properly seed the random number generator.
 */
state_t dice(double threshold) {
  double factor = drand48(); /**< Generate a random number in [0,1). */

  if (factor < threshold) {
    return STATE_CREATE_PACKET; /**< Transition to packet creation state. */
  } else {
    return STATE_NO_PACKET; /**< Remain in no-packet state. */
  }
}
