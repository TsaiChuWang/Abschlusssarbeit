#include <math.h>

/**
 * @file state_machine.h
 * @brief Header file defining the state machine for packet generation.
 *
 * This file contains the definition of the state machine, which controls the
 * state transitions for packet creation and non-creation.
 */
#define STATE_MACHINE_H

// #define STATE_CREATE_PACKET 1  /**  @brief State representing packet creation. */
// #define STATE_NO_PACKET 0   /**  @brief State representing no packet creation. */

#ifdef STATE_MACHINE_H /**< Include this block if STATE_MACHINE_H is defined */

int gcd(int a, int b)
{
  while (b != 0)
  {
    int temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

int is_naughty_index(int index, const configuration config)
{
  switch (config.naughty_mode)
  {
  case NAUGHTY_MODE_BEFORE:
    return (index >= (config.tenant_number - config.naughty_tenant_number));
    break;
  case NAUGHTY_MODE_AFTER:
    return (index < config.naughty_tenant_number);
    break;
  case NAUGHTY_MODE_AVERAGE:
    int gcd_ = gcd(config.tenant_number, config.naughty_tenant_number);
    if (gcd_ == 1)
    {
      printf(RED_ELOG "GCD of naughty_tenant_number and tenant number is 1!!\n" RESET);
      exit(EXIT_FAILURE);
    }
    int divisor = (int)(config.tenant_number / gcd_);
    return (index % divisor < (int)(config.naughty_tenant_number / gcd_));
    break;
  default:

    return (index >= (config.tenant_number - config.naughty_tenant_number));
    break;
  }
}

/**
 * @brief Defines states for packet processing.
 *
 * This enumeration represents the possible states of packet processing.
 */
typedef enum
{
  STATE_NO_PACKET,    /**< No packet is available. */
  STATE_CREATE_PACKET /**< A packet is being created. */
} state_t;

/**
 * @struct state_machine
 * @brief A structure representing the state machine for packet generation.
 *
 * This structure holds the current state and the probabilities of transitioning
 * between the states.
 */
typedef struct
{
  state_t state; /**< @brief Current state of the machine. Can be either STATE_CREATER_PACKET or STATE_NO_PACKET. */
  double r;      /**< @brief Probability of transitioning from STATE_CREATE_PACKET to STATE_CREATE_PACKET. */
  double p;      /**< @brief Probability of transitioning from STATE_NO_PACKET to STATE_NO_PACKET. */
} state_machine; /**< @brief Typedef for a state machine instance. */

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
  // if (state_r < generate_probability)
  // {
  //   printf(RED_ELOG "state_r is INVALID!\n" RESET);
  //   exit(EXIT_FAILURE);
  // }

  state_machine state; /**< Allocate memory for the state machine. */

  state.state = STATE_NO_PACKET; /**< Initial state is STATE_NO_PACKET. */
  state.r = state_r;             /**< Assign the provided state_r value. */
  state.p = (1.0 - (2 * generate_probability - generate_probability * state_r)) / (1.0 - generate_probability);
  // printf("p = %f\n", state.p);

  return state; /**< Return the initialized state machine. */
}

/**
 * @brief Initializes multiple state machines for a number of tenants.
 *
 * This function initializes a set of state machines based on the number of tenants.
 * Some tenants are initialized with a regular generate probability, while others
 * (naughty tenants) are initialized with a different probability. Each state machine
 * is initialized using the `initialize_state_machine` function.
 *
 * @param state_r The probability of staying in the current state (STATE_NO_PACKET) for regular tenants.
 * @param generate_probability The probability of transitioning to the STATE_CREATER_PACKET state for regular tenants.
 * @param generate_probability_naughty The probability of transitioning to the STATE_CREATER_PACKET state for naughty tenants.
 * @param tenant_number The total number of tenants.
 * @param naughty_tenant_number The number of naughty tenants.
 * @return A pointer to an array of initialized state_machine structures.
 */
state_machine *initialize_state_machines(double generate_probability, double generate_probability_naughty, const configuration config)
{
  state_machine *states = (state_machine *)malloc(sizeof(state_machine) * config.tenant_number); /**< Allocate memory for state machines. */

  for (int tenant = 0; tenant < config.tenant_number; tenant++)
  { /**< Iterate over all tenants. */
    if (is_naughty_index(tenant, config))
    {
      *(states + tenant) = initialize_state_machine(config.state_r, generate_probability_naughty); /**< Initialize naughty tenants with different probability. */
    }
    else
    {
      *(states + tenant) = initialize_state_machine(config.state_r, generate_probability); /**< Initialize regular tenants with default probability. */
    }
  }

  return states; /**< Return the array of state machines. */
}

/**
 * @brief Changes the state of a given state machine based on random probability.
 *
 * This function changes the state of the given state machine (`pstate`).
 * A random factor is generated, and the state transition is determined based
 * on the current state and transition probabilities (`p` and `r`).
 * The state changes according to the following logic:
 * - If in the STATE_CREATE_PACKET state, the transition to STATE_NO_PACKET occurs
 *   if the random factor is greater than the transition probability `r`.
 * - If in the STATE_NO_PACKET state, the transition to STATE_CREATE_PACKET occurs
 *   if the random factor is greater than the transition probability `p`.
 *
 * @param pstate Pointer to the state_machine structure to be updated.
 * @param random An integer value used as a seed to generate a random number.
 */
void change_state(state_machine *pstate, int random)
{
  // srand48(time(NULL)+ random);  // 只初始化一次隨機種子
  double factor = drand48();
  ; /**< Generate a random factor between 0 and 1. */

  if (pstate->state == STATE_CREATE_PACKET)
  { /**< Check if the current state is STATE_CREATE_PACKET. */
    if (factor < pstate->r)
    {
      pstate->state = STATE_CREATE_PACKET; /**< Stay in STATE_CREATE_PACKET. */
    }
    else
    {
      pstate->state = STATE_NO_PACKET; /**< Transition to STATE_NO_PACKET. */
    }
  }
  else
  { /**< If the current state is STATE_NO_PACKET. */
    if (factor < pstate->p)
    {
      pstate->state = STATE_NO_PACKET; /**< Stay in STATE_NO_PACKET. */
    }
    else
    {
      pstate->state = STATE_CREATE_PACKET; /**< Transition to STATE_CREATE_PACKET. */
    }
  }
}

/**
 * @brief Changes the state of all state machines in a given array of states.
 *
 * This function iterates through an array of state machines (`states`) and calls
 * the `change_state` function to update the state of each individual state machine.
 * The state transition is determined by each state machine's probabilities and the
 * random factor.
 *
 * @param states Pointer to an array of state_machine structures to be updated.
 * @param tenant_number The total number of tenants (state machines) whose states will be updated.
 */
void change_states(state_machine *states, int tenant_number)
{
  for (int tenant = 0; tenant < tenant_number; tenant++)
  {                                          /**< Iterate through each tenant. */
    change_state((states + tenant), tenant); /**< Change the state of each individual state machine. */
  }
}

/**
 * @brief Displays the state of a given state machine.
 *
 * This function prints the current state and transition probabilities (`r` and `p`)
 * of a given state machine. The state is displayed as either `STATE_CREATE_PACKET`
 * or `STATE_NO_PACKET` depending on the value of the state field.
 *
 * @param state The state_machine structure whose state and transition probabilities will be displayed.
 */
void show_state_machine(const state_machine state)
{
  printf("| current state     :");
  if (state.state == STATE_CREATE_PACKET)
    printf("STATE_CREATE_PACKET\n"); /**< Print the current state as STATE_CREATE_PACKET if true. */
  else
    printf("STATE_NO_PACKET\n"); /**< Print the current state as STATE_NO_PACKET if false. */

  printf("| r                 : %-.12f\n", state.r); /**< Print the transition probability 'r'. */
  printf("| p                 : %-.12f\n", state.p); /**< Print the transition probability 'p'. */
}

/**
 * @brief Prints the current state of all state machines in a given array.
 *
 * This function iterates through an array of state machines (`states`) and prints
 * the current state of each individual state machine. Each state is printed as either
 * `STATE_CREATE_PACKET` or `STATE_NO_PACKET` based on the state of the respective machine.
 *
 * @param states Pointer to an array of state_machine structures whose states will be printed.
 * @param tenant_number The total number of tenants (state machines) whose states will be printed.
 */
void print_current_states(state_machine *states, int tenant_number)
{
  for (int tenant = 0; tenant < tenant_number; tenant++) /**< Iterate through each tenant. */
    printf("%d", (states + tenant)->state);              /**< Print the state of each tenant's state machine. */
  printf("\n");                                          /**< Print a newline after displaying all states. */
}

#endif /**< End of inclusion guard for STATE_MACHINE_H */

// double state_r = 0.875;
// state_machine* states = initialize_state_machines(state_r, 0.75, 0.875, 10, 5);
// show_state_machine(*(states));
// show_state_machine(*(states+9));
// int* arr = calloc(10, sizeof(int));

// srand48(time(NULL));

// for(int i=0;i<327680;i++){
//     change_states(states, 10);
//     for(int j=0;j<10;j++)
//         if((states+j)->state == STATE_CREATE_PACKET)
//         *(arr+j) += 1;
//     // print_current_states(states, 10);
// }

// for(int j=0;j<10;j++)
//   printf("%d %f\n", *(arr+j), (double)*(arr+j)/327680);
