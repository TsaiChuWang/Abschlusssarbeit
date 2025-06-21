
#define GCRA_H

#ifdef GCRA_H

/**
 * @brief Structure representing a Generic Cell Rate Algorithm (GCRA) token bucket.
 */
typedef struct
{
    long tau;            /**< Time interval for token replenishment */
    long l;              /**< Bucket depth or allowable burst size */
    TIME_TYPE last_time; /**< Timestamp of the last packet arrival */
    long x;              /**< Current token count */
} GCRA;

/**
 * @brief Initializes a GCRA (Generic Cell Rate Algorithm) struct.
 *
 * @param tau The time interval for token replenishment.
 * @param l The bucket depth or allowable burst size.
 * @return A GCRA struct initialized with the given parameters.
 */
GCRA initializeGCRA(long tau, long l)
{
    GCRA gcra;

    gcra.tau = tau;
    gcra.l = l;
    gcra.last_time = 0;
    gcra.x = 0;

    return gcra;
}

/**
 * @brief Allocates and initializes an array of GCRA structures.
 *
 * @param tenant_number The number of tenants (or flows) requiring GCRA instances.
 * @param tau The time interval for token replenishment.
 * @param l The bucket depth or allowable burst size.
 * @return A pointer to an allocated array of GCRA structs.
 */
GCRA *initializeGCRAs(int tenant_number, long tau, long l)
{
    GCRA *gcras = (GCRA *)malloc(sizeof(GCRA) * tenant_number);
    if (!gcras)
    {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int index = 0; index < tenant_number; index++)
    {
        *(gcras + index) = initializeGCRA(tau, l);
    }
    return gcras;
}

/**
 * @brief Displays the properties of a given GCRA (Generic Cell Rate Algorithm) instance.
 *
 * @param gcra The GCRA instance to display.
 */
void show_GCRA(GCRA gcra)
{
    printf("tau       = %-ld\n", gcra.tau);
    printf("l         = %-ld\n", gcra.l);
    printf("last time = %-lf\n", gcra.last_time);
    printf("x         = %-ld\n", gcra.x);
}

/**
 * @brief Resets a GCRA instance by clearing its last time and accumulated value.
 *
 * @param pgcra Pointer to the GCRA instance to reset.
 */
void refresh_gcra(GCRA *pgcra)
{
    pgcra->last_time = 0;
    pgcra->x = 0;
}

/**
 * @brief Updates the GCRA (Generic Cell Rate Algorithm) state and determines whether a packet is accepted or labeled as GCRA-dropped.
 *
 * @param timestamp The current timestamp in nanoseconds.
 * @param pgcra Pointer to the GCRA instance being updated.
 * @param config Configuration parameters for traffic shaping.
 * @return PACKET_LABEL_GCRA_LABELED if the packet is dropped due to exceeding the threshold, otherwise PACKET_LABEL_ACCEPT.
 */
int gcra_update(TIME_TYPE timestamp, GCRA *pgcra, const configuration config)
{
    // Calculate the rate based on the time interval and mean traffic rate
    long rate = (long)(timestamp - pgcra->last_time) * (((double)(config.mean) * config.unit) / ONE_SECOND_IN_NS);
    long x = (long)(pgcra->x - rate);

#ifdef PRINT_GCRA_UPDATE
    // Compute upper bound for valid timestamp intervals
    double upper = (double)(ONE_SECOND_IN_NS * config.packet_size) / (config.mean * config.unit);
    if (timestamp - pgcra->last_time > (long)(upper))
        printf("lst = %9lf, time = %-7f, inter = %7lf, rate = %6ld x= %6ld\n",
               pgcra->last_time, timestamp, timestamp - pgcra->last_time, rate, x);
    else
        printf("lst = %9lf, time = %-7f, inter = \x1B[1;31m%6lf\x1B[0m, rate = %6ld x= %6ld\n",
               pgcra->last_time, timestamp, timestamp - pgcra->last_time, rate, x);

    printf("x = %ld, tau = %ld %d\n", x, pgcra->tau, x > pgcra->tau);
#endif

    // Check if the packet exceeds the threshold and should be dropped
    if (x > pgcra->tau)
    {
        return PACKET_LABEL_GCRA_LABELED;
    }
    else
    {
        // Update GCRA state and accept the packet
        pgcra->x = MAX((long)0, x) + pgcra->l;
        pgcra->last_time = timestamp;
        return PACKET_LABEL_ACCEPT;
    }

    return PACKET_LABEL_ACCEPT; // Redundant but kept for safety
}

int gcra_update_advanced(TIME_TYPE timestamp, GCRA *pgcra, const configuration config, int index)
{
    // Calculate the rate based on the time interval and mean traffic rate
    long rate = 0;
    if (index < 20)
        rate = (long)(timestamp - pgcra->last_time) * (((double)120 * config.unit) / ONE_SECOND_IN_NS);
    if (index > 19 && index < 40)
        rate = (long)(timestamp - pgcra->last_time) * (((double)140 * config.unit) / ONE_SECOND_IN_NS);
    if (index > 39 && index < 50)
        rate = (long)(timestamp - pgcra->last_time) * (((double)80 * config.unit) / ONE_SECOND_IN_NS);
    if (index > 49 && index < 80)
        rate = (long)(timestamp - pgcra->last_time) * (((double)160 * config.unit) / ONE_SECOND_IN_NS);
    if (index > 79 && index < 100)
        rate = (long)(timestamp - pgcra->last_time) * (((double)130 * config.unit) / ONE_SECOND_IN_NS);

    long x = (long)(pgcra->x - rate);

#ifdef PRINT_GCRA_UPDATE
    // Compute upper bound for valid timestamp intervals
    double upper = (double)(ONE_SECOND_IN_NS * config.packet_size) / (config.mean * config.unit);
    if (timestamp - pgcra->last_time > (long)(upper))
        printf("lst = %9lf, time = %-7f, inter = %7lf, rate = %6ld x= %6ld\n",
               pgcra->last_time, timestamp, timestamp - pgcra->last_time, rate, x);
    else
        printf("lst = %9lf, time = %-7f, inter = \x1B[1;31m%6lf\x1B[0m, rate = %6ld x= %6ld\n",
               pgcra->last_time, timestamp, timestamp - pgcra->last_time, rate, x);

    printf("x = %ld, tau = %ld %d\n", x, pgcra->tau, x > pgcra->tau);
#endif

    // Check if the packet exceeds the threshold and should be dropped
    if (x > pgcra->tau)
    {
        return PACKET_LABEL_GCRA_LABELED;
    }
    else
    {
        // Update GCRA state and accept the packet
        pgcra->x = MAX((long)0, x) + pgcra->l;
        pgcra->last_time = timestamp;
        return PACKET_LABEL_ACCEPT;
    }

    return PACKET_LABEL_ACCEPT; // Redundant but kept for safety
}

#endif // GCRA_H
