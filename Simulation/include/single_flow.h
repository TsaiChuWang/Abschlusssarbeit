#define SINGLE_FLOW_H

#ifdef SINGLE_FLOW_H

typedef struct
{
    int index;              /**< Index of the row configuration */
    int traffic_mode;       /**< Mode of traffic (e.g., 0 for normal, 1 for high, etc.) */
    int mean;               /**< Mean value for traffic generation */
    int standard_deviation; /**< Standard deviation for traffic generation */
    int packet_size;        /**< Size of packets in bytes */
    int real_traffic;       /**< Actual traffic value */
    double state_r;         /**< State variable (could represent a ratio or probability) */
    int FIFO_queue_buffer;  /**< Size of the FIFO queue buffer */
    long tau;               /**< Time constant or delay in some context */

    double generate_probability; /**< Packet generation probability for compliant traffic */

    state_machine states; /**< Array of state machines for traffic generation */
    GCRA gcra;            /**< GCRA (Generic Cell Rate Algorithm) instance */
    meter_queue mqueue;   /**< Meter queue for managing packets */
    int dequeue_count;    /**< Count of packets dequeued */

    int packet_count;       /**< Total number of packets processed */
    int FIFO_drop_count;    /**< Count of packets dropped in the FIFO queue */
    int GCRA_labeled_count; /**< Count of packets labeled by GCRA */
    int link_dropped_count; /**< Count of links dropped */

    double loss; /**< Loss rate of packets */
} single_flow;

/**
 * @brief Initializes a single flow configuration.
 *
 * This function sets up a single_flow structure based on the provided row_configuration
 * and common_configuration. It initializes various parameters, including traffic mode,
 * state machines, and GCRA settings. It also handles error cases for unsupported traffic modes.
 *
 * @param flow Pointer to the single_flow structure to be initialized.
 * @param rconfig Pointer to the row_configuration containing traffic parameters.
 * @param index Pointer to an integer representing the current index, which will be incremented.
 * @param cconfig The common_configuration containing unit and input rate for calculations.
 */
void init_single_flow(single_flow *flow, row_configuration *rconfig, int *index, common_configuration cconfig)
{
    flow->index = *index;                                   // Set the index of the flow
    *index++;                                               // Increment the index for the next flow
    flow->traffic_mode = rconfig->traffic_mode;             // Set traffic mode
    flow->mean = rconfig->mean;                             // Set mean value for traffic generation
    flow->standard_deviation = rconfig->standard_deviation; // Set standard deviation
    flow->packet_size = rconfig->packet_size;               // Set packet size
    flow->real_traffic = rconfig->real_traffic;             // Set actual traffic value
    flow->state_r = rconfig->state_r;                       // Set state variable
    flow->FIFO_queue_buffer = rconfig->FIFO_queue_buffer;   // Set FIFO queue buffer size
    flow->tau = rconfig->tau;                               // Set time constant

    // Calculate packet generation probability
    flow->generate_probability =
        (double)flow->real_traffic * cconfig.unit / cconfig.input_rate;

    // Initialize state machine based on traffic mode
    if (flow->traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION)
        flow->states = initialize_state_machine(0.5, flow->generate_probability);
    else if (flow->traffic_mode == TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL)
        flow->states = initialize_state_machine(flow->state_r, flow->generate_probability);
    else
    {
        printf(RED_ELOG "init_single_flow error : traffic mode unfound\n" RESET);
        exit(EXIT_FAILURE); // Exit on unsupported traffic mode
    }

    // Initialize GCRA and meter queue
    flow->gcra = initializeGCRA(flow->tau, flow->packet_size);
    flow->mqueue = init_meter_queue_advanced(*rconfig, cconfig.unit);
    flow->dequeue_count = 0; // Initialize dequeue count

    // Initialize packet-related counts
    flow->packet_count = 0;
    flow->FIFO_drop_count = 0;
    flow->GCRA_labeled_count = 0;
    flow->link_dropped_count = 0;

    double loss = 0; // Initialize loss variable (not used further)
}

#endif