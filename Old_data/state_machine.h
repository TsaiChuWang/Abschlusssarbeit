/**
 * @file state_machine.h
 * @brief Header file for state machine implementation for packet processing
 * @details Defines the state machine structure and states for packet generation
 *          and processing, including transition probabilities.
 */

#include <math.h>

#ifndef CONFIG_H
#include "../include/configuration.h"
#endif

#define STATE_MACHINE_H

#ifdef STATE_MACHINE_H

/**
 * @brief Defines states for packet processing
 * @details This enumeration represents the possible states in the packet
 *          processing state machine. The machine can either be in a state
 *          where no packet is being processed or where a packet is being created.
 */
typedef enum
{
  STATE_NO_PACKET,    /**< No packet is currently being processed */
  STATE_CREATE_PACKET /**< System is in the process of creating a packet */
} state_t;

/**
 * @struct state_machine
 * @brief State machine structure for packet generation
 * @details This structure represents a two-state Markov chain for packet
 *          generation, with associated transition probabilities.
 *
 * The state transitions are governed by the following probabilities:
 * - p: Probability of staying in STATE_NO_PACKET
 * - (1-p): Probability of transitioning from STATE_NO_PACKET to STATE_CREATE_PACKET
 * - r: Probability of staying in STATE_CREATE_PACKET
 * - (1-r): Probability of transitioning from STATE_CREATE_PACKET to STATE_NO_PACKET
 *
 * Constraints:
 * - 0 <= p <= 1
 * - 0 <= r <= 1
 */
typedef struct
{
  state_t state; /**< Current state of the machine */
  double r;      /**< Probability of staying in STATE_CREATE_PACKET */
  double p;      /**< Probability of staying in STATE_NO_PACKET */
} state_machine;

/**
 * @note The following functions should be implemented for state machine operations:
 * - state_machine_init(): Initialize the state machine
 * - state_machine_update(): Update the state based on transition probabilities
 * - state_machine_get_state(): Get the current state

/**
* @brief Initializes a state machine for packet generation.
*
* This function initializes a state machine with the given probabilities
* and initial state. The state machine starts in the STATE_NO_PACKET state.
* The probabilities of transitioning between states are calculated based on
* the given `state_r` and `generate_probability`.
*
* @param state_r The probability of staying in the current state (STATE_NO_PACKET).
* @param generate_probability The probability of transitioning to the STATE_CREATER_PACKET state.
* @return A state_machine structure with the initialized values.
*/
state_machine initialize_state_machine(double state_r, double generate_probability)
{
  if (state_r < generate_probability)
  {
    printf(RED_ELOG "state_r is INVALID!\n" RESET);
    exit(EXIT_FAILURE);
  }

  state_machine state; /**< Allocate memory for the state machine. */

  state.state = STATE_NO_PACKET; /**< Initial state is STATE_NO_PACKET. */
  state.r = state_r;             /**< Assign the provided state_r value. */
  state.p = (1.0 - (2 * generate_probability - generate_probability * state_r)) / (1.0 - generate_probability);
  // printf("p = %f\n", state.p);

  return state; /**< Return the initialized state machine. */
}

/**
 * @brief Initializes multiple state machines for a number of tenants
 * @details Creates and initializes state machines for both compliant and noncompliant
 *          tenants with different transition probabilities based on configuration.
 *          The state machines are used to simulate packet generation behavior.
 *
 * @param[in] generate_probability Transition probability for compliant tenants
 *                                (STATE_NO_PACKET to STATE_CREATE_PACKET)
 * @param[in] generate_probability_noncompliant Transition probability for noncompliant tenants
 *                                             (STATE_NO_PACKET to STATE_CREATE_PACKET)
 * @param[in] config Configuration structure containing:
 *                   - tenant_number: Total number of tenants
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 *                   - state_r: Regular state retention probability
 *                   - noncompliant_state_r: Noncompliant state retention probability
 *                   - traffic_mode: Traffic generation mode
 *
 * @return Pointer to array of initialized state_machine structures
 * @retval NULL Memory allocation failed
 *
 * @note Memory is dynamically allocated and must be freed by the caller
 *
 * @warning Ensure config parameters are valid before calling:
 *          - tenant_number > 0
 *          - noncompliant_tenant_number <= tenant_number
 *          - 0 <= probabilities <= 1
 *
 * State machine initialization rules:
 * - Regular tenants:
 *   - Use state_r and generate_probability
 * - Noncompliant tenants:
 *   - If TRAFFIC_MODE_DIFFERENT_R: Use noncompliant_state_r
 *   - Otherwise: Use state_r
 *   - Always use generate_probability_noncompliant
 */
state_machine *initialize_state_machines(double generate_probability, double generate_probability_noncompliant, const configuration config)
{
  /* Allocate memory for state machines */
  state_machine *states = (state_machine *)malloc(sizeof(state_machine) * config.tenant_number); /**< Allocate memory for state machines. */

  /* Initialize each tenant's state machine */
  for (int tenant = 0; tenant < config.tenant_number; tenant++)
  { /**< Iterate over all tenants. */
    if (is_noncompliant_index(tenant, config))
    {
      /* Initialize noncompliant tenant state machine */
      if (config.traffic_mode >= TRAFFIC_MODE_DIFFERENT_R)
        *(states + tenant) = initialize_state_machine(config.noncompliant_state_r, generate_probability_noncompliant); /**< Initialize noncompliant tenants with different probability. */
      else
        *(states + tenant) = initialize_state_machine(config.state_r, generate_probability_noncompliant); /**< Initialize noncompliant tenants with different probability. */
    }
    else
    {
      /* Initialize compliant tenant state machine */
      *(states + tenant) = initialize_state_machine(config.state_r, generate_probability); /**< Initialize compliant tenants with default probability. */
    }
  }

  return states; /**< Return the array of state machines. */
}

/**
 * @brief Changes the state of a state machine based on probabilistic rules
 * @details Implements a two-state Markov chain with the following transitions:
 *          - STATE_CREATE_PACKET to STATE_NO_PACKET: probability (1 - r)
 *          - STATE_CREATE_PACKET to STATE_CREATE_PACKET: probability r
 *          - STATE_NO_PACKET to STATE_CREATE_PACKET: probability (1 - p)
 *          - STATE_NO_PACKET to STATE_NO_PACKET: probability p
 *
 * State transition diagram:
 * @verbatim
 *                    r
 *           ┌─────────────────┐
 *           │                 │
 *           ▼                 │
 *    ┌──────────────┐   ┌────────────────┐
 *    │              │   │                │
 *    │ STATE_CREATE │   │   STATE_NO     │
 *    │   PACKET    │◄──│    PACKET      │
 *    │              │   │                │
 *    └──────────────┘   └────────────────┘
 *           ▲                 │
 *           │                 │
 *           └─────────────────┘
 *                  1-p
 * @endverbatim
 *
 * @param[in,out] pstate Pointer to state machine to update
 * @param[in] random Seed value for random number generation
 *
 * @note Uses drand48() for random number generation
 * @warning Ensure pstate is not NULL before calling
 */
void change_state(state_machine *pstate, int random)
{
  /* Input validation */
  if (pstate == NULL)
  {
    return;
  }

  /* Generate random factor [0,1] */
  double factor = drand48();

  /* State transition logic */
  if (pstate->state == STATE_CREATE_PACKET)
  {
    /* Currently in CREATE_PACKET state */
    pstate->state = (factor < pstate->r) ? STATE_CREATE_PACKET : /* Stay with probability r */
                        STATE_NO_PACKET;                         /* Transition with probability 1-r */
  }
  else
  {
    /* Currently in NO_PACKET state */
    pstate->state = (factor < pstate->p) ? STATE_NO_PACKET : /* Stay with probability p */
                        STATE_CREATE_PACKET;                 /* Transition with probability 1-p */
  }
}

/**
 * @brief Displays detailed information about a state machine
 * @details Prints a formatted output showing:
 *          - Current state (STATE_CREATE_PACKET or STATE_NO_PACKET)
 *          - Retention probability (r)
 *          - Generation probability (p)
 *
 * Output format:
 * @verbatim
 * | current state     : STATE_CREATE_PACKET
 * | r                 : 0.123456789012
 * | p                 : 0.123456789012
 * @endverbatim
 *
 * @param[in] state State machine structure to display
 *
 * @note Probabilities are displayed with 12 decimal places
 */

void change_states(state_machine *states, int tenant_number)
{
  /* Input validation */
  if (states == NULL || tenant_number <= 0)
  {
    return;
  }

  /* Update each state machine */
  for (int tenant = 0; tenant < tenant_number; tenant++)
  {
    change_state(&states[tenant], tenant);
  }
}

/**
 * @brief Displays detailed information about a state machine
 * @details Prints a formatted output showing:
 *          - Current state (STATE_CREATE_PACKET or STATE_NO_PACKET)
 *          - Retention probability (r)
 *          - Generation probability (p)
 *
 * Output format:
 * @verbatim
 * | current state     : STATE_CREATE_PACKET
 * | r                 : 0.123456789012
 * | p                 : 0.123456789012
 * @endverbatim
 *
 * @param[in] state State machine structure to display
 *
 * @note Probabilities are displayed with 12 decimal places
 */
void show_state_machine(const state_machine state)
{
  /* Display current state */
  printf("| current state     : %s\n",
         state.state == STATE_CREATE_PACKET ? "STATE_CREATE_PACKET" : "STATE_NO_PACKET");

  /* Display transition probabilities */
  printf("| r                 : %-.12f\n", state.r);
  printf("| p                 : %-.12f\n", state.p);
}

/**
 * @brief Displays current states of multiple state machines
 * @details Prints a compact representation of all state machines' states
 *          in a single line. Each state is represented as a binary digit:
 *          - 1: STATE_CREATE_PACKET
 *          - 0: STATE_NO_PACKET
 *
 * Example output for 8 tenants:
 * @verbatim
 * 10110010
 * @endverbatim
 *
 * @param[in] states Array of state machines to display
 * @param[in] tenant_number Number of state machines in the array
 *
 * @warning Ensure states is not NULL and tenant_number > 0
 *
 * Example usage:
 * @code
 * state_machine *states = initialize_state_machines(...);
 * int tenant_number = 8;
 * print_current_states(states, tenant_number);
 * @endcode
 */
void print_current_states(state_machine *states, int tenant_number)
{
  /* Input validation */
  if (states == NULL || tenant_number <= 0)
  {
    printf(RED_ELOG "Error: Invalid input parameters\n" RESET);
    return;
  }

  /* Print state of each machine */
  for (int tenant = 0; tenant < tenant_number; tenant++)
  {
    printf("%d", states[tenant].state);
  }
  printf("\n");
}

/**
 * @brief Prints detailed state information for all machines
 * @details Combines show_state_machine() with visual separators for better readability
 *
 * @param[in] states Array of state machines
 * @param[in] tenant_number Number of state machines
 */
void print_detailed_states(state_machine *states, int tenant_number)
{
  if (states == NULL || tenant_number <= 0)
  {
    printf("Error: Invalid input parameters\n");
    return;
  }

  print_equals_line();
  for (int tenant = 0; tenant < tenant_number; tenant++)
  {
    printf("Tenant %d:\n", tenant);
    show_state_machine(states[tenant]);
    print_equals_line();
  }
}

#endif /**< End of inclusion guard for STATE_MACHINE_H */
