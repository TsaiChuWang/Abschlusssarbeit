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
    *index += 1;                                            // Increment the index for the next flow
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
        flow->states = initialize_state_machine(1, 0.5);
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

/**
 * @brief Displays the details of a single flow configuration.
 *
 * This function prints the attributes of a single_flow structure, including traffic mode,
 * mean, standard deviation, packet size, real traffic, and other relevant statistics.
 * It also displays the generate probability and various counts related to packet processing.
 *
 * @param flow The single_flow structure containing the flow configuration to be displayed.
 */
void show_single_flow(single_flow flow)
{
    printf(" index | traffic mode | mean | deviation | packet | real | state_r | FIFO |   tau  |    loss   |\n");

    // Display flow details based on traffic mode
    if (flow.traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION)
    {
        printf("  %2d   |    uniform   |  %3d |    %3d    |  %3d   |  %3d | %.5f | %3d  |  %5ld | %2.7f |\n",
               flow.index, flow.mean, flow.standard_deviation, flow.packet_size, flow.real_traffic, flow.state_r,
               flow.FIFO_queue_buffer, flow.tau, flow.loss);
    }
    else
    {
        printf("  %2d   |     burst    |  %3d |    %3d    |  %3d   |  %3d | %.5f | %3d  |  %5ld | %2.7f |\n",
               flow.index, flow.mean, flow.standard_deviation, flow.packet_size, flow.real_traffic, flow.state_r,
               flow.FIFO_queue_buffer, flow.tau, flow.loss);
    }

    // Print generate probability and other statistics
    printf("generate probability : %-2.7lf\n", flow.generate_probability);
    print_equals_line(); // Print a separator line

    // Display state machine details if applicable
    if (flow.traffic_mode == TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL)
        show_state_machine(flow.states);

    // Show GCRA details
    show_GCRA(flow.gcra);
    print_equals_line(); // Print another separator line

    // Print additional flow statistics
    printf("generate probability : %-2.7lf\n", flow.generate_probability);
    printf("dequeue count        : %-7d\n", flow.dequeue_count);
    printf("packet count         : %-7d\n", flow.packet_count);
    printf("FIFO drop count      : %-7d\n", flow.FIFO_drop_count);
    printf("GCRA labeled count   : %-7d\n", flow.GCRA_labeled_count);
    printf("link dropped count   : %-7d\n", flow.link_dropped_count);
    printf("loss                 : %-2.7lf\n", flow.loss);
}

/**
 * @brief Creates a packet for a single flow based on the traffic mode.
 *
 * This function determines whether to create a packet based on the flow's traffic mode.
 * It uses either a uniform distribution or an on-off model to decide if a packet should be generated.
 *
 * @param flow Pointer to the single_flow structure representing the flow configuration.
 * @return int Returns a packet label indicating whether a packet was created or not.
 *              Possible return values:
 *              - PACKET_LABEL_ACCEPT: A packet was created.
 *              - PACKET_LABEL_NO_PACKET: No packet was created.
 */
int create_packet_single_flow(single_flow *flow)
{
    int packet = PACKET_LABEL_NO_PACKET; // Initialize packet label to indicate no packet

    // Determine packet creation based on traffic mode
    if (flow->traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION)
    {
        packet = uniform_distribution(flow->generate_probability); // Use uniform distribution
    }
    else if (flow->traffic_mode == TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL)
    {
        change_state(&(flow->states), flow->index); // Change state based on the flow's state machine
        packet = (flow->states.state == STATE_CREATE_PACKET)
                     ? PACKET_LABEL_ACCEPT     // Accept packet creation
                     : PACKET_LABEL_NO_PACKET; // No packet created
    }

    // Increment packet count if a packet was created
    if (packet == PACKET_LABEL_ACCEPT)
    {
        flow->packet_count += 1;
    }

    return packet; // Return the packet label
}

/**
 * @brief Manages the FIFO queue meter for a single flow.
 *
 * This function processes the meter queue for a flow, handling both dequeuing and enqueuing of packets
 * based on the current timestamp. It updates the counts of dequeued packets and manages packet drops
 * when the queue is full.
 *
 * @param flow Pointer to the single_flow structure representing the flow configuration.
 * @param current_timestamp The current time used to determine when to dequeue packets.
 * @return int Returns a packet label indicating the result of the operation.
 *              Possible return values:
 *              - PACKET_LABEL_ACCEPT: A packet was successfully enqueued.
 *              - PACKET_LABEL_OVER_UPPERBOUND_DROPPED: A packet was dropped due to queue overflow.
 */
int FIFO_queue_meter_single_flow(single_flow *flow, TIME_TYPE current_timestamp)
{
    int packet = PACKET_LABEL_NO_PACKET; // Initialize packet label to indicate no packet

    // Dequeue packets based on the current timestamp
    while (flow->mqueue.dequeue_timestamp <= current_timestamp)
    {
        flow->mqueue.dequeue_timestamp += flow->mqueue.dequeue_interval; // Update dequeue timestamp
        flow->dequeue_count += 1;                                        // Increment the count of dequeued packets
    }

    // Check if there are packets to dequeue for the tenant
    if (flow->dequeue_count > 0)
    {
        meter_dequeue(&(flow->mqueue)); // Dequeue a packet from the meter queue
        flow->dequeue_count -= 1;       // Decrement the count of dequeued packets
    }

    // Enqueue a packet if the meter queue is ready
    if (meter_enqueue(&(flow->mqueue)) == QUEUE_READY)
    {
        packet = PACKET_LABEL_ACCEPT; // Set the packet label to ACCEPT
    }
    else
    {
        flow->FIFO_drop_count += 1;                    // Increment drop count if queue is not ready
        packet = PACKET_LABEL_OVER_UPPERBOUND_DROPPED; // Return drop label
    }

    // Dequeue remaining packets if any
    while (flow->dequeue_count > 0)
    {
        meter_dequeue(&(flow->mqueue)); // Dequeue a packet from the meter queue
        flow->dequeue_count -= 1;       // Decrement the count of dequeued packets
    }
    return packet; // Return the packet label
}

/**
 * @brief Updates the GCRA (Generic Cell Rate Algorithm) for a single flow.
 *
 * This function updates the flow's GCRA state based on the current timestamp
 * and increments the labeled packet count if a packet is labeled by GCRA.
 *
 * @param flow Pointer to the single_flow structure representing the flow configuration.
 * @param current_timestamp The current time used to update the GCRA.
 * @param config Common configuration parameters affecting the GCRA update.
 * @return int Returns a packet label indicating the result of the GCRA update.
 *              Possible return values:
 *              - PACKET_LABEL_NO_PACKET: No packet was generated.
 *              - PACKET_LABEL_GCRA_LABELED: A packet was labeled by GCRA.
 */
int GCRA_update_single_flow(single_flow *flow, TIME_TYPE current_timestamp, common_configuration config)
{
    int packet = PACKET_LABEL_NO_PACKET; // Initialize packet label to indicate no packet

    // Update GCRA state and check for packet labeling
    packet = gcra_update_advanced(current_timestamp, &(flow->gcra), flow->mean, config);
    if (packet == PACKET_LABEL_GCRA_LABELED)
    {
        flow->GCRA_labeled_count += 1; // Increment the count of labeled packets
    }

    return packet; // Return the packet label
}

/**
 * @brief Updates the state of a single flow and processes packets.
 *
 * This function attempts to create a packet for the given flow and updates
 * its state based on the current timestamp and configuration. It uses FIFO
 * queuing and GCRA (Generic Cell Rate Algorithm) for flow control.
 *
 * @param flow Pointer to the single flow structure to be updated.
 * @param current_timestamp The current time for processing the flow.
 * @param config Configuration settings for flow management.
 * @return int Packet label indicating the result of the update operation.
 */
int three_update(single_flow *flow, TIME_TYPE current_timestamp, common_configuration config) {
    int packet = PACKET_LABEL_NO_PACKET; // Initialize packet label to indicate no packet

    // Attempt to create a packet for the flow
    packet = create_packet_single_flow(flow);
    if (packet == PACKET_LABEL_NO_PACKET) {
        return PACKET_LABEL_NO_PACKET; // No packet created, exit early
    }

    // Process the packet through FIFO queueing
    packet = FIFO_queue_meter_single_flow(flow, current_timestamp);
    if (packet == PACKET_LABEL_OVER_UPPERBOUND_DROPPED) {
        return PACKET_LABEL_OVER_UPPERBOUND_DROPPED; // Packet dropped due to upper bound
    }

    // Update the flow state using GCRA
    packet = GCRA_update_single_flow(flow, current_timestamp, config);
    return packet; // Return the result of the GCRA update
}

/**
 * @brief Prints the packet counts for a single flow.
 *
 * This function formats and prints the packet statistics for a given flow,
 * including the total packet count, FIFO drop count, GCRA labeled count,
 * and link drop count.
 *
 * @param flow Pointer to the single_flow structure containing packet statistics.
 */
void print_packet_count_single_flow(single_flow *flow) {
    char mode = '-';
    if(flow->traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION)
        mode = 'u';
    if(flow->traffic_mode == TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL)
        mode = 'b';
    
    char tag = '-';
    if(flow->real_traffic > flow->mean)
        tag = 'n';
    else
        tag = 'c';

    printf("   %-2d  |   %c   |  %c  | %-8d | %-8d | %-8d | %-8d | %-.7lf |\n", 
           flow->index, 
           mode,
           tag,
           flow->packet_count, 
           flow->FIFO_drop_count, 
           flow->GCRA_labeled_count, 
           flow->link_dropped_count,
           flow->loss);
}

/**
 * @brief Prints the packet counts for multiple flows.
 *
 * This function prints the header and iterates through an array of flows,
 * calling print_packet_count_single_flow for each flow to display its statistics.
 *
 * @param flows Pointer to an array of single_flow structures.
 * @param flow_number The number of flows in the array.
 */
void print_packet_count(single_flow *flows, int flow_number) {
    // Print the header for the packet statistics
    printf(" index | mode  | SLA |  Packet  |   FIFO   |   GCRA   |   Link   |    Loss   |\n");
    printf("------------------------------------------------------------------------------\n");
    
    // Iterate through the array of flows and print each flow's statistics
    for (int i = 0; i < flow_number; i++) {
        print_packet_count_single_flow(&flows[i]);
    }
}

/**
 * @brief Calculates the loss rate for a single flow.
 *
 * This function computes the loss rate as the sum of link drops and FIFO drops
 * divided by the total packet count. If the packet count is zero, the loss rate
 * is set to zero to avoid division by zero.
 *
 * @param flow Pointer to the single_flow structure for which to calculate loss.
 */
void count_loss_single_flow(single_flow *flow) {
    if (flow->packet_count > 0) {
        flow->loss = (double)(flow->link_dropped_count + flow->FIFO_drop_count) / flow->packet_count;
    } else {
        flow->loss = 0.0; // Set loss to 0 if there are no packets to avoid division by zero
    }

    flow->loss = flow->loss*100;
}

/**
 * @brief Calculates the loss rates for multiple flows.
 *
 * This function iterates through an array of flows and calculates the loss
 * rate for each flow by calling count_loss_single_flow.
 *
 * @param flows Pointer to an array of single_flow structures.
 * @param flow_number The number of flows in the array.
 */
void count_loss(single_flow *flows, int flow_number) {
    for (int i = 0; i < flow_number; i++) {
        count_loss_single_flow(&flows[i]);
    }
}

/**
 * @brief Updates the loss for a specific row configuration based on the flows.
 *
 * This function calculates the loss for a row configuration by averaging
 * the losses of the relevant flows within the specified range.
 *
 * @param rconfig Pointer to the row_configuration to update.
 * @param flows Pointer to an array of single_flow structures.
 * @param config Pointer to the common_configuration containing tenant information.
 */
void update_loss_row_config(row_configuration* rconfig, single_flow* flows, common_configuration *config) {
    double loss = 0.0;
    int count = 0; // To count valid flows in the range

    for (int i = 0; i < config->tenant_number; i++) {
        if (i <= rconfig->end_index && i >= rconfig->start_index) {
            loss += (flows + i)->loss; // Accumulate loss
            count++; // Increment count of valid flows
        }
    }

    // Calculate average loss if there are valid flows
    if (count > 0) {
        loss /= count;
    }
    rconfig->loss = loss; // Update the row configuration with the calculated loss
}

/**
 * @brief Updates the CSV configuration with loss statistics based on the flows.
 *
 * This function calculates various types of loss statistics for the CSV configuration,
 * including average loss, compliant loss, non-compliant loss, uniform loss, and burst loss.
 *
 * @param csv_config Pointer to the csv_configuration to update.
 * @param flows Pointer to an array of single_flow structures.
 * @param config Pointer to the common_configuration containing tenant information.
 */
void update_loss_csv_config(csv_configuration* csv_config, single_flow* flows, common_configuration *config) {
    double average_loss = 0.0;
    double compliant_loss = 0.0;
    double non_compliant_loss = 0.0;
    double uniform_loss = 0.0;
    double burst_loss = 0.0;

    for (int i = 0; i < csv_config->kind_number; i++) {
        update_loss_row_config((csv_config->rows + i), flows, config);

        // Calculate average loss
        average_loss += (csv_config->rows + i)->loss * ((csv_config->rows + i)->number / (double)config->tenant_number);

        // Calculate compliant loss
        if ((csv_config->rows + i)->real_traffic <= (csv_config->rows + i)->mean && 
            (config->tenant_number - csv_config->noncompliant_number) > 0) {
            compliant_loss += (csv_config->rows + i)->loss * ((csv_config->rows + i)->number / (double)(config->tenant_number - csv_config->noncompliant_number));
        } else if (csv_config->noncompliant_number > 0) {
            non_compliant_loss += (csv_config->rows + i)->loss * ((csv_config->rows + i)->number / (double)csv_config->noncompliant_number);
        }

        // Calculate uniform loss
        if ((csv_config->rows + i)->traffic_mode == TRAFFIC_MODE_ADVANCED_UNIFORM_DISTRIBUTION && csv_config->uniform_number > 0) {
            uniform_loss += (csv_config->rows + i)->loss * ((csv_config->rows + i)->number / (double)csv_config->uniform_number);
        }

        // Calculate burst loss
        if ((csv_config->rows + i)->traffic_mode == TRAFFIC_MODE_ADVANCED_ON_OFF_MODEL && csv_config->burst_number > 0) {
            burst_loss += (csv_config->rows + i)->loss * ((csv_config->rows + i)->number / (double)csv_config->burst_number);
        }
    }

    // Update the CSV configuration with calculated losses
    csv_config->average_loss = average_loss;
    csv_config->compliant_loss = compliant_loss;
    csv_config->non_compliant_loss = non_compliant_loss;
    csv_config->uniform_loss = uniform_loss;
    csv_config->burst_loss = burst_loss;
}
#endif