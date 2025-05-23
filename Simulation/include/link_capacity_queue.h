#define LINK_CAPACITY_QUEUE_H

// /**
//  * @def PRINT_QUEUE_FULL
//  *
//  * @brief Enables printing of messages when the queue is full or a value is dropped.
//  *
//  * When `PRINT_QUEUE_FULL` is defined, the `enqueue` function will print a message
//  * indicating that the queue is full or when a value is dropped from the queue.
//  * This is helpful for debugging and understanding queue behavior during simulation.
//  */
// // #define PRINT_QUEUE_FULL

#ifdef LINK_CAPACITY_QUEUE_H

#define QUEUE_FULL_STATE -1
#define QUEUE_READY 1

/**
 * @enum queue_code_t
 * @brief Represents different queue codes.
 */
typedef enum
{
    ALPHA, /**< High-priority queue */
    BETA   /**< Low-priority queue */
} queue_code_t;

/**
 * @struct circular_queue
 * @brief Represents a circular queue structure.
 * @details This structure is used to implement a circular queue with a fixed buffer size.
 */
typedef struct
{
    int *items;     /**< Pointer to the array storing queue elements */
    int front;      /**< Index of the front element */
    int rear;       /**< Index of the rear element */
    int size;       /**< Current number of elements in the queue */
    int max_buffer; /**< Maximum buffer size of the queue */
} circular_queue;

/**
 * @brief Traffic metering queue structure with rate control
 *
 * @details This structure implements a rate-controlled queue system that combines
 *          a circular queue with timing mechanisms for traffic shaping.
 *          It provides controlled packet dequeuing based on specified time intervals.
 *
 * Key Features:
 * - Circular buffer for packet storage
 * - Time-based dequeuing control
 * - Rate limiting capabilities
 * - Thread-safe operations (if implemented)
 *
 * @struct meter_queue
 */
typedef struct
{
    /**
     * @brief Underlying circular queue for packet storage
     *
     * @details Implements a circular FIFO buffer for storing packets.
     *          Properties:
     *          - Fixed size buffer
     *          - FIFO ordering
     *          - Wrap-around capability
     *          - Overflow protection
     *
     * @see circular_queue for detailed implementation
     */
    circular_queue queue;

    /**
     * @brief Timestamp of the last dequeue operation
     *
     * @details Records the system time when the last packet was dequeued.
     *          Used to enforce minimum time intervals between dequeue operations.
     *
     * Usage:
     * - Rate control calculations
     * - Interval enforcement
     * - Timing statistics
     */
    TIME_TYPE dequeue_timestamp;

    /**
     * @brief Minimum time interval between dequeue operations
     *
     * @details Specifies the minimum time that must elapse between
     *          consecutive dequeue operations. Used to implement
     *          rate limiting.
     *
     * Calculation:
     * - interval = 1 / target_rate
     * - Units depend on TIME_TYPE definition
     */
    TIME_TYPE dequeue_interval;
} meter_queue;

/**
 * @struct link_priority_queue
 * @brief Represents a link priority queue with two internal queues.
 * @details This structure contains two circular queues (alpha and beta) for managing prioritized traffic.
 */
typedef struct
{
    circular_queue alpha; /**< High-priority queue (Alpha) */
    circular_queue beta;  /**< Low-priority queue (Beta) */
    int max_buffer;       /**< Maximum buffer size for the queues */

    TIME_TYPE dequeue_timestamp; /**< Timestamp of the last dequeue operation */
    TIME_TYPE dequeue_interval;  /**< Time interval for packet dequeueing */
} link_priority_queue;

/**
 * @brief Initializes a circular queue.
 *
 * Allocates memory for the queue and sets its initial values.
 *
 * @param cqueue Pointer to the circular queue structure.
 * @param max_buffer_size Maximum buffer size of the queue.
 */
void init_circular_queue(circular_queue *cqueue, int max_buffer_size)
{
    cqueue->items = (int *)malloc(max_buffer_size * sizeof(int));
    memset(cqueue->items, UNFOUND, max_buffer_size * sizeof(int));
    cqueue->front = 0;
    cqueue->rear = 0;
    cqueue->size = 0;
    cqueue->max_buffer = max_buffer_size;
}

/**
 * @brief Initializes a meter queue for a given configuration.
 *
 * This function creates and initializes a meter_queue structure based on the
 * provided configuration. It sets up a circular queue for each tenant and
 * calculates the dequeue interval based on packet size and timing parameters.
 *
 * @param config The configuration structure containing parameters for initialization.
 * @return meter_queue The initialized meter_queue structure.
 */
meter_queue init_meter_queue(const configuration config)
{
    meter_queue mqueue = {0}; ///< Initialize the meter queue structure to zero.

    for (int i = 0; i < config.tenant_number; i++)
    {
        init_circular_queue(&(mqueue.queue), config.upper_queue_buffer); ///< Initialize the circular queue.

        mqueue.dequeue_timestamp = (TIME_TYPE)0; ///< Initialize the dequeue timestamp to zero.
        mqueue.dequeue_interval = (TIME_TYPE)(config.packet_size *
                                              (double)ONE_SECOND_IN_NS / ((config.mean + config.standard_deviation) * config.unit)); ///< Calculate the dequeue interval.
    }

    return mqueue; ///< Return the initialized meter queue.
}

/**
 * @brief Initializes multiple meter queues for each tenant.
 *
 * This function allocates memory for an array of meter_queue structures,
 * initializing each one based on the provided configuration. It sets up
 * meter queues for all tenants specified in the configuration.
 *
 * @param config The configuration structure containing parameters for initialization.
 * @return meter_queue* Pointer to the array of initialized meter_queue structures.
 */
meter_queue *init_meter_queues(const configuration config)
{
    meter_queue *mqueues = (meter_queue *)malloc(sizeof(meter_queue) * config.tenant_number); ///< Allocate memory for meter queues.
    for (int i = 0; i < config.tenant_number; i++)
    {
        *(mqueues + i) = init_meter_queue(config); ///< Initialize each meter queue.
    }
    return mqueues; ///< Return the pointer to the array of meter queues.
}

// /**
//  * @brief Initializes a priority queue.
//  *
//  * This function initializes two circular queues (alpha and beta) and
//  * sets up the dequeue interval based on packet size and bandwidth.
//  *
//  * @param pqueue Pointer to the priority queue structure.
//  * @param max_buffer_size Maximum buffer size for each queue.
//  * @param config Configuration containing packet size and unit conversion.
//  * @param bandwidth Available bandwidth in bits per second.
//  */
// void initlink_priority_queue(link_priority_queue *pqueue, int max_buffer_size,
//                              const configuration config, double bandwidth)
// {
//     init_circular_queue(&pqueue->alpha, max_buffer_size);
//     init_circular_queue(&pqueue->beta, max_buffer_size);
//     pqueue->max_buffer = max_buffer_size;

//     pqueue->dequeue_timestamp = 0;
//     pqueue->dequeue_interval = (TIME_TYPE)(config.packet_size *
//                                            (double)ONE_SECOND_IN_NS / (bandwidth * config.unit));
// }

/**
 * @brief Checks if a circular queue is empty.
 *
 * A queue is considered empty when its size is zero.
 *
 * @param cqueue Pointer to the circular queue structure.
 * @return int Returns 1 if the queue is empty, otherwise 0.
 */
int is_empty(circular_queue *cqueue)
{
    return cqueue->size == 0;
}

/**
 * @brief Checks if a circular queue is full.
 *
 * A queue is considered full when its size reaches the maximum buffer size.
 *
 * @param cqueue Pointer to the circular queue structure.
 * @return int Returns 1 if the queue is full, otherwise 0.
 */
int is_full(circular_queue *cqueue)
{
    return cqueue->size == cqueue->max_buffer;
}

// /**
//  * @brief Gets the total size of both queues in a priority queue.
//  *
//  * This function returns the sum of the sizes of the alpha and beta queues.
//  *
//  * @param pqueue Pointer to the priority queue structure.
//  * @return int Total number of elements in both queues.
//  */
// int total_size(link_priority_queue *pqueue)
// {
//     return pqueue->alpha.size + pqueue->beta.size;
// }

// /**
//  * @brief Dequeues an element from the priority queue.
//  *
//  * This function attempts to dequeue an element from the alpha queue first.
//  * If the alpha queue is empty, it dequeues from the beta queue. If both queues
//  * are empty, it returns a special value `UNFOUND`.
//  *
//  * @param pqueue Pointer to the priority queue structure.
//  * @return int The dequeued value from either the alpha or beta queue, or `UNFOUND` if both queues are empty.
//  */
// int dequeue(link_priority_queue *pqueue)
// {
//     if (!is_empty(&pqueue->alpha))
//     {
//         int value = pqueue->alpha.items[pqueue->alpha.front];
//         pqueue->alpha.front = (pqueue->alpha.front + 1) % pqueue->max_buffer;
//         pqueue->alpha.size--;
//         return value;
//     }
//     else if (!is_empty(&pqueue->beta))
//     {
//         int value = pqueue->beta.items[pqueue->beta.front];
//         pqueue->beta.front = (pqueue->beta.front + 1) % pqueue->max_buffer;
//         pqueue->beta.size--;
//         return value;
//     }
//     return UNFOUND; // Queue is empty
// }

void cdequeue(circular_queue *cqueue)
{
    // printf("c=%d\n", cqueue->size);
    if (is_empty(cqueue))
    {
        return;
    }
    int value = cqueue->items[cqueue->front];
    cqueue->front = (cqueue->front + 1) % cqueue->max_buffer;
    cqueue->size--;
}

void meter_dequeue(meter_queue *mqueue)
{
    cdequeue(&(mqueue->queue));
}

int cenqueue(circular_queue *cqueue)
{
    if (is_full(cqueue))
    {
        return QUEUE_FULL_STATE;
    }
    else
    {
        cqueue->rear = (cqueue->rear + 1) % cqueue->max_buffer;
        cqueue->size++;
        return QUEUE_READY;
    }
}

int meter_enqueue(meter_queue *mqueue)
{
    return cenqueue(&(mqueue->queue));
}

// /**
//  * @brief Enqueues a value into the priority queue.
//  *
//  * This function enqueues a value into either the alpha (high priority) or beta
//  * (low priority) queue, depending on the provided `code`. If the respective queue
//  * is full, it will drop the new value or an existing value from the other queue
//  * to make space.
//  *
//  * @param pqueue Pointer to the priority queue structure.
//  * @param code The queue to enqueue the value into (either BETA or ALPHA).
//  * @param value The value to be enqueued.
//  * @param drop_value Pointer to store the value that was dropped, if any.
//  *                   Set to `UNFOUND` if no value is dropped.
//  *
//  * @note The function may drop a value from the beta queue if the total size
//  *       of both queues exceeds the maximum buffer size.
//  * @note If `PRINT_QUEUE_FULL` is defined, a message will be printed when the
//  *       queue is full or when a value is dropped.
//  */
// void enqueue(link_priority_queue *pqueue, queue_code_t code, int value, int *drop_value)
// {
//     *drop_value = UNFOUND;

//     if (code == BETA)
//     { // Beta queue (low priority)
//         if (total_size(pqueue) >= pqueue->max_buffer)
//         {
//             *drop_value = value; // Drop the new value
//         }
//         else
//         {
//             pqueue->beta.items[pqueue->beta.rear] = value;
//             pqueue->beta.rear = (pqueue->beta.rear + 1) % pqueue->max_buffer;
//             pqueue->beta.size++;
//         }
//     }
//     else
//     { // Alpha queue (high priority)
//         if (is_full(&pqueue->alpha))
//         {
//             *drop_value = value;
// #ifdef PRINT_QUEUE_FULL
//             printf("Queue is full for value %d\n", value);
// #endif
//         }
//         else if (total_size(pqueue) >= pqueue->max_buffer)
//         {
//             *drop_value = pqueue->beta.items[pqueue->beta.front]; // Drop from Beta queue
// #ifdef PRINT_QUEUE_FULL
//             printf("Drop beta queue value %d\n", *drop_value);
// #endif
//             pqueue->beta.front = (pqueue->beta.front + 1) % pqueue->max_buffer;
//             pqueue->beta.size--;

//             pqueue->alpha.items[pqueue->alpha.rear] = value;
//             pqueue->alpha.rear = (pqueue->alpha.rear + 1) % pqueue->max_buffer;
//             pqueue->alpha.size++;
//         }
//         else
//         {
//             pqueue->alpha.items[pqueue->alpha.rear] = value;
//             pqueue->alpha.rear = (pqueue->alpha.rear + 1) % pqueue->max_buffer;
//             pqueue->alpha.size++;
//         }
//     }
// }

// /**
//  * @brief Prints the contents of both the alpha and beta queues for debugging.
//  *
//  * This function prints the elements in the alpha (high priority) and beta (low priority)
//  * queues, followed by the total size of both queues combined.
//  * The output is used for debugging and understanding the current state of the queues.
//  *
//  * @param pqueue Pointer to the priority queue structure containing both the alpha and beta queues.
//  */
// void print_queue(link_priority_queue *pqueue)
// {
//     printf("Alpha Queue: ");
//     for (int i = 0; i < pqueue->alpha.size; i++)
//         printf("%d ", pqueue->alpha.items[(pqueue->alpha.front + i) % pqueue->max_buffer]);

//     printf("\nBeta Queue: ");
//     for (int i = 0; i < pqueue->beta.size; i++)
//         printf("%d ", pqueue->beta.items[(pqueue->beta.front + i) % pqueue->max_buffer]);

//     printf("\ntotal_size = %d\n", total_size(pqueue));
//     printf("\n");
// }

#endif