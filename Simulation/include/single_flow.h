#define SINGLE_FLOW_H

#ifdef SINGLE_FLOW_H

typedef struct
{
    int index;              // Index of the row configuration
    int traffic_mode;       // Mode of traffic (e.g., 0 for normal, 1 for high, etc.)
    int mean;               // Mean value for traffic generation
    int standard_deviation; // Standard deviation for traffic generation
    int packet_size;        // Size of packets in bytes
    int real_traffic;       // Actual traffic value
    double state_r;         // State variable (could represent a ratio or probability)
    int FIFO_queue_buffer;  // Size of the FIFO queue buffer
    long tau;               // Time constant or delay in some context

    double generate_probability; /**< Packet generation probability for compliant traffic */

    state_machine states; /**< Array of state machines for traffic generation */
    GCRA gcra;
    meter_queue mqueue;
    int dequeue_count;

    int packet_count;
    int FIFO_drop_count;
    int GCRA_labeled_count;
    int link_dropped_count;

    double loss;
} single_flow;

#endif