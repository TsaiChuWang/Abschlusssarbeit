/**
 * @file packets_count.h
 * @brief Header file for packet counting functions and structures.
 * This file defines the structure and functions related to packet counting for tenants.
 */

/**
 * @def RECORD_HEADER
 * @brief Enable record headers for various statistics logging
 * @details When defined, includes all header definitions for statistical records
 */
#define RECORD_HEADER

// #define TAG_PURE_LOSS

#ifdef RECORD_HEADER

/**
 * @defgroup StatisticsHeaders Statistics Recording Headers
 * @brief Header definitions for various statistics recording formats
 * @{
 */

/**
 * @def RECORD_COMPLIANT_AND_NONCOMPLIANT_TAU_HEADER
 * @brief CSV header for basic compliance statistics
 * @details Format specification:
 * - tau: Time constant
 * - compliant_loss: Packet loss rate for compliant traffic
 * - noncompliant_loss: Packet loss rate for non-compliant traffic
 */
#define RECORD_COMPLIANT_AND_NONCOMPLIANT_TAU_HEADER \
  "tau,compliant_loss,noncompliant_loss\n"

/**
 * @def RECORD_COMPLIANT_AND_NONCOMPLIANT_ALL_HEADER
 * @brief CSV header for detailed compliance statistics
 * @details Format specification:
 * - tau: Time constant
 * - noncompliant_mean: Average non-compliance rate
 * - noncompliant_tenant_number: Number of non-compliant tenants
 * - state_r: System state parameter
 * - noncompliant_state_r: Non-compliant state parameter
 * - upper_queue_buffer: Upper queue buffer size
 * - link_queue_buffer: Link queue buffer size
 * - compliant_loss: Compliant packet loss rate
 * - noncompliant_loss: Non-compliant packet loss rate
 */
#define RECORD_COMPLIANT_AND_NONCOMPLIANT_ALL_HEADER           \
  "tau,noncompliant_mean,noncompliant_tenant_number,state_r,"  \
  "noncompliant_state_r,upper_queue_buffer,link_queue_buffer," \
  "compliant_loss,noncompliant_loss\n"

/**
 * @def RECORD_AVERAGE_LOSS_HEADER
 * @brief CSV header for average loss statistics
 * @details Format specification:
 * - tau: Time constant
 * - noncompliant_mean: Average non-compliance rate
 * - noncompliant_tenant_number: Number of non-compliant tenants
 * - state_r: System state parameter
 * - noncompliant_state_r: Non-compliant state parameter
 * - upper_queue_buffer: Upper queue buffer size
 * - link_queue_buffer: Link queue buffer size
 * - average_loss: Average packet loss rate
 */
#define RECORD_AVERAGE_LOSS_HEADER                             \
  "tau,noncompliant_mean,noncompliant_tenant_number,state_r,"  \
  "noncompliant_state_r,upper_queue_buffer,link_queue_buffer," \
  "average_loss\n"

/**
 * @def RECORD_PACKET_SITUATION_HEADER
 * @brief CSV header for packet processing statistics
 * @details Format specification:
 * - packets: Number of packets processed
 * - dequeue: Number of dequeue operations
 */
#define RECORD_PACKET_SITUATION_HEADER "packets,dequeue\n"

/** @} */ // end of StatisticsHeaders group

/**
 * @brief Header type enumeration
 */
enum HeaderType
{
  HEADER_TYPE_TAU = 0, ///< Basic compliance statistics
  HEADER_TYPE_ALL,     ///< Detailed compliance statistics
  HEADER_TYPE_AVERAGE, ///< Average loss statistics
  HEADER_TYPE_PACKET   ///< Packet situation statistics
};

/**
 * @brief Helper function to write headers to output files
 * @param[in] file_ptr File pointer to write headers to
 * @param[in] header_type Type of header to write
 * @return int Success status (0 for success, FAILURE for failure)
 */
int write_statistics_header(FILE *file_ptr, int header_type)
{
  if (!file_ptr)
    return FAILURE;

  switch (header_type)
  {
  case HEADER_TYPE_TAU:
    fprintf(file_ptr, RECORD_COMPLIANT_AND_NONCOMPLIANT_TAU_HEADER);
    break;
  case HEADER_TYPE_ALL:
    fprintf(file_ptr, RECORD_COMPLIANT_AND_NONCOMPLIANT_ALL_HEADER);
    break;
  case HEADER_TYPE_AVERAGE:
    fprintf(file_ptr, RECORD_AVERAGE_LOSS_HEADER);
    break;
  case HEADER_TYPE_PACKET:
    fprintf(file_ptr, RECORD_PACKET_SITUATION_HEADER);
    break;
  default:
    return UNFOUND;
  }
  return SUCCESS;
}

#endif // RECORD_HEADER

#define PACKETS_COUNT_H

#ifdef PACKETS_COUNT_H

/**
 * @struct packets_count
 * @brief Structure to hold packet count statistics for tenants
 * @details Tracks packet statistics including counts per tenant and grid metrics
 */
typedef struct
{
  long *count;       /**< Array holding the packet count for each tenant */
  int tenant_number; /**< The number of tenants in the system */
  long grid_length;  /**< The grid length (total number of events) */
} packets_count;     /**< Typedef for packets_count structure */

/**
 * @brief Initializes the packet count structure
 *
 * @details This function performs the following operations:
 * - Validates input parameters
 * - Allocates memory for packet counting
 * - Initializes all counters to zero
 * - Sets up tenant and grid parameters
 *
 * @param[out] pcount Pointer to the packets_count structure to be initialized
 * @param[in] tenant_number Number of tenants (must be positive)
 * @param[in] grid_length Length of the grid (must be positive)
 *
 * @return int Status code (0 for success, error code otherwise)
 *
 * @note The caller is responsible for calling free_packets_count() when done
 *
 * @see free_packets_count()
 */
int init_packets_count(packets_count *pcount, int tenant_number, long grid_length)
{
  // Input validation
  if (pcount == NULL)
  {
    printf(RED_ELOG "init_packets_count : PCOUNT NULL\n" RESET);
    return FAILURE;
  }

  if (tenant_number <= 0)
  {
    printf(RED_ELOG "init_packets_count : TENANT_NUMBER IS ILEGAL\n" RESET);
    return FAILURE;
  }

  if (grid_length <= 0)
  {
    printf(RED_ELOG "init_packets_count : GRID_LENGTH IS NEGATIVE\n" RESET);
    return FAILURE;
  }

  // Initialize structure to known state
  pcount->count = NULL;
  pcount->tenant_number = 0;
  pcount->grid_length = 0;

  // Allocate memory for packet counts
  pcount->count = (long *)calloc(tenant_number, sizeof(long));
  if (pcount->count == NULL)
  {
    printf(RED_ELOG "init_packets_count : COUNT CALLOC FAILED\n" RESET);
    return FAILURE;
  }

  // Set structure parameters
  pcount->tenant_number = tenant_number;
  pcount->grid_length = grid_length;

  return SUCCESS;
}

/**
 * @brief Frees resources associated with a packets_count structure
 *
 * @param[in,out] pcount Pointer to the packets_count structure to free
 */
void free_packets_count(packets_count *pcount)
{
  if (pcount != NULL)
  {
    free(pcount->count);
    pcount->count = NULL;
    pcount->tenant_number = 0;
    pcount->grid_length = 0;
  }
}

/**
 * @brief Displays the packet count statistics
 *
 * This function prints the packet count for each tenant along with the percentage
 * of total packets for each tenant. It includes separator lines for better readability.
 *
 * @param[in] count The packets_count structure containing the statistics
 * @return void
 */
void show_packets_count(packets_count count)
{
  // 輸入驗證
  if (count.count == NULL || count.tenant_number <= 0 || count.grid_length <= 0)
  {
    printf("Error: Invalid packets_count structure\n");
    return;
  }

  // 上方分隔線
  print_equals_line();
  printf("Packet Count Statistics:\n");

  // 顯示每個租戶的統計資料
  for (int tenant = 0; tenant < count.tenant_number; tenant++)
  {
    long packet_count = count.count[tenant];
    double percentage = (double)(packet_count * 100.0) / count.grid_length;

    printf("Tenant %-3d | Packets: %-10ld | Percentage: %.2f%%\n",
           tenant,
           packet_count,
           percentage);
  }

  // 下方分隔線
  print_equals_line();
}

/**
 * @struct packets_label
 * @brief Structure to hold the packet labels and associated counts for tenants.
 * This structure stores the labels for each tenant, such as ACCEPT, DROP_OVER, DROP_GCRA, and CAPACITY,
 * as well as the count of packets for each label and the total number of tenants.
 */
typedef struct
{
  int **labels;      /**< 2D array holding the labels for each tenant (ACCEPT, DROP_OVER, DROP_GCRA, CAPACITY). */
  long *count;       /**< Array holding the packet count for each tenant. */
  int tenant_number; /**< The number of tenants. */
} packets_label;

/**
 * @brief Initializes the packets_label structure.
 *
 * @details This function performs the following initialization steps:
 *          - Allocates and initializes memory for the count array
 *          - Copies packet counts from the provided packets_count structure
 *          - Allocates memory for the 2D labels array
 *          - Initializes each tenant's label array to zero
 *
 * @param[out] plabel      Pointer to the packets_label structure to be initialized
 * @param[in]  tenant_number The number of tenants
 * @param[in]  pcount     Pointer to the packets_count structure containing initial packet counts
 *
 * @pre pcount must contain valid packet counts for tenant_number tenants
 * @pre PACKET_LABEL_TYPE must be defined and greater than 0
 *
 * @note Memory allocated by this function must be freed using an appropriate cleanup function
 *
 * @warning No parameter validation is performed; ensure all parameters are valid before calling
 */
void init_Packets_Label(packets_label *plabel, int tenant_number, packets_count *pcount)
{
  plabel->count = (long *)calloc(tenant_number, sizeof(long));        /**< Allocate memory for the packet counts. */
  memcpy(plabel->count, pcount->count, tenant_number * sizeof(long)); /**< Copy the packet counts from the provided structure. */

  plabel->labels = (int **)malloc(tenant_number * sizeof(int *)); /**< Allocate memory for the labels array. */
  for (int tenant = 0; tenant < tenant_number; tenant++)
    *(plabel->labels + tenant) = (int *)calloc(PACKET_LABEL_TYPE_NUMBER, sizeof(int)); /**< Allocate memory for each tenant's labels. */

  plabel->tenant_number = tenant_number; /**< Set the tenant number. */
}

/**
 * @brief Frees the memory allocated for the packet label structure
 *
 * @param[in,out] plabel Pointer to the packets_label structure to be freed
 */
void free_packets_label(packets_label *plabel)
{
  if (plabel != NULL)
  {
    if (plabel->labels != NULL)
    {
      // 釋放每個租戶的標籤陣列
      for (int i = 0; i < plabel->tenant_number; i++)
      {
        free(plabel->labels[i]);
      }
      free(plabel->labels);
      plabel->labels = NULL;
    }

    // 釋放計數陣列
    free(plabel->count);
    plabel->count = NULL;

    // 重設租戶數量
    plabel->tenant_number = 0;
  }
}

/**
 * @brief Displays the packet label statistics.
 *
 * @details This function displays detailed statistics for each tenant including:
 *          - ACCEPT: Number of accepted packets
 *          - DROP_OVER: Number of packets dropped due to overflow
 *          - DROP_LABELED: Number of packets dropped by GCRA
 *          - CAPACITY: Capacity limit
 *          - LOSS(PURE): Percentage of capacity-related packet loss
 *          - LOSS(ALL): Percentage of total packet loss (including overflow)
 *
 * The function also calculates and displays the following averages across all tenants:
 *          - Average exceed (overflow drops)
 *          - Average GCRA drops
 *          - Average pure loss percentage
 *          - Average total loss percentage
 *
 * @param[in] label The packets_label structure containing the statistics data
 *
 * @note The output is formatted in a table-like structure with colored average statistics
 * @note LOSS(PURE) = (CAPACITY / (ACCEPT + CAPACITY)) * 100%
 * @note LOSS(ALL) = ((DROP_OVER + CAPACITY) / (ACCEPT + DROP_OVER + CAPACITY)) * 100%
 *
 * @pre RED_ELOG and RESET color macros must be defined
 * @pre print_equals_line() function must be available
 */
void show_packets_label(packets_label label)
{
  double average_exceed = 0.0;
  double average_gcra = 0.0;
  double average_loss_p = 0.0;
  double average_loss_a = 0.0;

  print_equals_line(); /**< Print a separator line. */
  printf("label =\n");
  printf("%-10s, %-10s, %-10s, %-10s : %-12s % , %-12s %\n", "ACCEPT", "DROP_OVER", "DROP_LABELED", "CAPACITY", "LOSS(PURE)", "LOSS(ALL)");

  // Loop through each tenant and display the label counts and loss percentages.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
  {
    printf("%-10d, %-10d, %-12d, %-10d : %-12.7f % , %-12.7f %\n",
           label.labels[tenant][PACKET_LABEL_ACCEPT], label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED], label.labels[tenant][PACKET_LABEL_GCRA_LABELED], label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED],
           (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]),
           (double)(label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]));

    average_exceed += label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED];                                                                                                                                                                                                                                                    /**< Sum the exceed labels. */
    average_gcra += label.labels[tenant][PACKET_LABEL_GCRA_LABELED];                                                                                                                                                                                                                                                                 /**< Sum the GCRA labels. */
    average_loss_p += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]);                                                                                                                           /**< Sum the pure loss percentages. */
    average_loss_a += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Sum the all loss percentages. */
  }

  // Display the average statistics.
  printf(RED_ELOG " average exceed     = %-f\n" RESET, average_exceed / label.tenant_number);
  printf(RED_ELOG " average GCRA       = %-f\n" RESET, average_gcra / label.tenant_number);
  printf(RED_ELOG " average loss(pure) = %-f\n" RESET, average_loss_p / label.tenant_number);
  printf(RED_ELOG " average loss(all)  = %-f\n" RESET, average_loss_a / label.tenant_number);

  print_equals_line(); /**< Print a separator line. */
}

/**
 * @brief Prints the compliant and noncompliant tenant statistics.
 *
 * @details This function analyzes and displays separate statistics for compliant and noncompliant tenants:
 *          - Average GCRA drops per tenant type
 *          - Average packet loss percentage per tenant type
 *
 *          The function performs the following steps:
 *          1. Validates traffic mode compatibility
 *          2. Checks for presence of noncompliant tenants
 *          3. Calculates separate statistics for each tenant type
 *          4. Displays averaged results
 *
 * @param[in] label  The packets_label structure containing packet statistics
 * @param[in] config Configuration structure containing:
 *                   - traffic_mode: Current traffic mode
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 *
 * @note Loss percentage calculation: (CAPACITY / (ACCEPT + CAPACITY)) * 100%
 *
 * @pre RED_ELOG and RESET color macros must be defined
 * @pre is_noncompliant_index() function must be available
 *
 * @return void, but will exit early with error message if:
 *         - Traffic mode is incompatible (UNIFORM, DENSITY, or ALL_NONCOMPLIANT_UNIFORM)
 *         - No noncompliant tenants are configured (noncompliant_tenant_number == 0)
 *
 * @warning Function assumes tenants are ordered with compliant tenants first,
 *          followed by noncompliant tenants
 */
void print_compliant_and_noncompliant(packets_label label, const configuration config)
{
  if (config.traffic_mode == TRAFFIC_MODE_UNIFORM || config.traffic_mode == TRAFFIC_MODE_DENSITY || config.traffic_mode == TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM)
  {
    printf(RED_ELOG "print_compliant_and_noncompliant : TRAFFIC MODE DOES NOT SUPPORT\n" RESET);
    return;
  }

  if (config.noncompliant_tenant_number == 0)
  {
    printf(RED_ELOG "print_compliant_and_noncompliant : NONCOMPLIANT TENANT NUMBER IS ZERO\n" RESET);
    return;
  }

  double noncompliant_GCRA = 0;
  double noncompliant_Loss = 0;
  double compliant_GCRA = 0;
  double compliant_Loss = 0;

  // Loop through each tenant and accumulate statistics based on their type (compliant or noncompliant).
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (is_noncompliant_index(tenant, config))
    {
      /**< Accumulate GCRA for noncompliant tenants. */
      noncompliant_GCRA += label.labels[tenant][PACKET_LABEL_GCRA_LABELED];
#ifdef TAG_PURE_LOSS
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }
    else
    {
      /**< Accumulate GCRA for compliant tenants. */
      compliant_GCRA += label.labels[tenant][PACKET_LABEL_GCRA_LABELED];
#ifdef TAG_PURE_LOSS
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) * 100.0 / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }

  // Print the GCRA and loss statistics for compliant and noncompliant tenants.
  printf("compliant    : GCRA : %-7.12f Loss :%-7.12f% \n", compliant_GCRA / (label.tenant_number - config.noncompliant_tenant_number), compliant_Loss / (label.tenant_number - config.noncompliant_tenant_number));
  printf("noncompliant : GCRA : %-7.12f Loss :%-7.12f% \n", noncompliant_GCRA / config.noncompliant_tenant_number, noncompliant_Loss / config.noncompliant_tenant_number);
}

/**
 * @brief Records the compliant and noncompliant tenant loss statistics in a CSV file.
 *
 * @details This function:
 *          1. Validates traffic mode and noncompliant tenant configuration
 *          2. Calculates average packet loss for both compliant and noncompliant tenants
 *          3. Appends results to "compliant_and_noncompliant_tau.csv" with format:
 *             tau, compliant_loss, noncompliant_loss
 *
 * The loss percentage is calculated as: CAPACITY / (ACCEPT + CAPACITY)
 *
 * @param[in] label  The packets_label structure containing packet statistics
 * @param[in] config Configuration structure containing:
 *                   - traffic_mode: Current traffic mode
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 *                   - tau: Current tau value for logging
 *                   - data_path: Directory path for output file
 *
 * @pre RED_ELOG and RESET color macros must be defined
 * @pre MAX_PATH_LENGTH must be defined
 * @pre is_noncompliant_index() function must be available
 * @pre data_path directory must exist and be writable
 *
 * @return void, but will exit in following cases:
 *         - Early return with error message if:
 *           * Traffic mode is incompatible (UNIFORM, DENSITY, or ALL_NONCOMPLIANT_UNIFORM)
 *           * No noncompliant tenants are configured
 *         - Program termination if:
 *           * File cannot be opened for writing
 *
 * @note Output CSV format:
 *       tau_value, compliant_loss_average, noncompliant_loss_average
 *
 * @warning Function assumes tenants are ordered with compliant tenants first,
 *          followed by noncompliant tenants
 * @warning Existing file will be appended to, not overwritten
 */
void record_compliant_and_noncompliant_tau(packets_label label, const configuration config)
{

  if (config.traffic_mode == TRAFFIC_MODE_UNIFORM || config.traffic_mode == TRAFFIC_MODE_DENSITY || config.traffic_mode == TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM)
  {
    printf(RED_ELOG "record_compliant_and_noncompliant_tau : TRAFFIC MODE DOES NOT SUPPORT\n" RESET);
    return;
  }

  if (config.noncompliant_tenant_number == 0)
  {
    printf(RED_ELOG "record_compliant_and_noncompliant_tau : NONCOMPLIANT TENANT NUMBER IS ZERO\n" RESET);
    return;
  }

  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/compliant_and_noncompliant_tau.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file"); /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double noncompliant_Loss = 0;
  double compliant_Loss = 0;

  // Loop through each tenant and calculate the loss percentage for compliant and noncompliant tenants.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (is_noncompliant_index(tenant, config))
    {
#ifdef TAG_PURE_LOSS
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }
    else
    {
#ifdef TAG_PURE_LOSS
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }

  // Append the results to the CSV file.
  fprintf(file, "%ld, %f, %f\n", config.tau, compliant_Loss, noncompliant_Loss);
  fclose(file); /**< Close the file after writing. */
}

/**
 * @brief Records detailed statistics for compliant and noncompliant tenants in a CSV file.
 *
 * @details This function:
 *          1. Validates traffic mode and noncompliant tenant configuration
 *          2. Calculates average packet loss for both tenant types
 *          3. Appends comprehensive statistics to "compliant_and_noncompliant_all.csv"
 *
 * The loss percentage is calculated as: CAPACITY / (ACCEPT + CAPACITY)
 *
 * @param[in] label  The packets_label structure containing packet statistics
 * @param[in] config Configuration structure containing:
 *                   - tau: Current tau value
 *                   - noncompliant_mean: Mean value for noncompliant tenants
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 *                   - state_r: State parameter for compliant tenants
 *                   - noncompliant_state_r: State parameter for noncompliant tenants
 *                   - upper_queue_buffer: Upper queue buffer size
 *                   - link_queue_buffer: Link queue buffer size
 *                   - data_path: Directory path for output file
 *                   - traffic_mode: Current traffic mode
 *
 * @pre RED_ELOG and RESET color macros must be defined
 * @pre MAX_PATH_LENGTH must be defined
 * @pre is_noncompliant_index() function must be available
 * @pre data_path directory must exist and be writable
 *
 * @return void, but will exit in following cases:
 *         - Early return with error message if:
 *           * Traffic mode is incompatible (UNIFORM, DENSITY, or ALL_NONCOMPLIANT_UNIFORM)
 *           * No noncompliant tenants are configured
 *         - Program termination if:
 *           * File cannot be opened for writing
 *
 * @note Output CSV format:
 *       tau, noncompliant_mean, noncompliant_tenant_number, state_r, noncompliant_state_r,
 *       upper_queue_buffer, link_queue_buffer, compliant_loss, noncompliant_loss
 *
 * @warning Function assumes tenants are ordered with compliant tenants first,
 *          followed by noncompliant tenants
 * @warning Existing file will be appended to, not overwritten
 * @warning All configuration parameters must be properly initialized before calling
 */
void record_compliant_and_noncompliant_all(packets_label label, const configuration config)
{

  if (config.traffic_mode == TRAFFIC_MODE_UNIFORM || config.traffic_mode == TRAFFIC_MODE_DENSITY || config.traffic_mode == TRAFFIC_MODE_ALL_NONCOMPLIANT_UNIFORM)
  {
    printf(RED_ELOG "record_compliant_and_noncompliant_all : TRAFFIC MODE DOES NOT SUPPORT\n" RESET);
    return;
  }

  if (config.noncompliant_tenant_number == 0)
  {
    printf(RED_ELOG "record_compliant_and_noncompliant_all : NONCOMPLIANT TENANT NUMBER IS ZERO\n" RESET);
    return;
  }

  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/compliant_and_noncompliant_all.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file"); /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double noncompliant_Loss = 0;
  double compliant_Loss = 0;

  // Loop through each tenant and calculate the loss percentage for compliant and noncompliant tenants.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (is_noncompliant_index(tenant, config))
    {
#ifdef TAG_PURE_LOSS
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      noncompliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }
    else
    {
#ifdef TAG_PURE_LOSS
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
      compliant_Loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
    }

  // Append the results to the CSV file with additional configuration details.
  fprintf(file, "%ld, %d, %d, %f, %f, %d, %d, %f, %f\n", config.tau, config.noncompliant_mean, config.noncompliant_tenant_number, config.state_r, config.noncompliant_state_r, config.upper_queue_buffer, config.link_queue_buffer, compliant_Loss, noncompliant_Loss);
  fclose(file); /**< Close the file after writing. */
}

/**
 * @brief Records the average loss statistics across all tenants in a CSV file.
 *
 * @details This function:
 *          1. Calculates the average packet loss across all tenants
 *          2. Appends results to "record_average_loss.csv" with configuration details
 *
 * The loss percentage for each tenant is calculated as:
 * CAPACITY / (ACCEPT + CAPACITY)
 *
 * The final average is the sum of individual loss percentages.
 *
 * @param[in] label  The packets_label structure containing:
 *                   - tenant_number: Total number of tenants
 *                   - labels[][]: Array of packet statistics per tenant
 *                     [tenant][PACKET_LABEL_ACCEPT] = accepted packets
 *                     [tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] = dropped packets
 * @param[in] config Configuration structure containing:
 *                   - tau: Current tau value
 *                   - noncompliant_mean: Mean value for noncompliant tenants
 *                   - noncompliant_tenant_number: Number of noncompliant tenants
 *                   - state_r: State parameter for compliant tenants
 *                   - noncompliant_state_r: State parameter for noncompliant tenants
 *                   - upper_queue_buffer: Upper queue buffer size
 *                   - link_queue_buffer: Link queue buffer size
 *                   - data_path: Directory path for output file
 *
 * @pre MAX_PATH_LENGTH must be defined
 * @pre data_path directory must exist and be writable
 * @pre label.tenant_number must be > 0
 * @pre All configuration parameters must be properly initialized
 *
 * @return void, but will terminate program if:
 *         - File cannot be opened for writing
 *
 * @note Output CSV format:
 *       tau, noncompliant_mean, noncompliant_tenant_number, state_r,
 *       noncompliant_state_r, upper_queue_buffer, link_queue_buffer, average_loss
 *
 * @warning Existing file will be appended to, not overwritten
 * @warning The average loss is a simple sum of loss percentages, not weighted by traffic volume
 * @warning Function does not validate configuration parameters before use
 *
 * @see record_compliant_and_noncompliant_all() for related statistics by tenant type
 */
void record_average_loss(packets_label label, const configuration config)
{
  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/record_average_loss.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file"); /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double average_loss = 0.0;

  // Loop through each tenant and calculate the average loss.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
  {
#ifdef TAG_PURE_LOSS
    average_loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#else
    average_loss += (double)(label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED] + label.labels[tenant][PACKET_LABEL_OVER_UPPERBOUND_DROPPED]) / (label.labels[tenant][PACKET_LABEL_ACCEPT] + label.labels[tenant][PACKET_LABEL_OVER_CAPACITY_DROPPED]); /**< Calculate average loss percentage. */
#endif
  }

  // Append the results to the CSV file.
  fprintf(file, "%ld, %d, %d, %f, %f, %d, %d, %f\n", config.tau, config.noncompliant_mean, config.noncompliant_tenant_number, config.state_r, config.noncompliant_state_r, config.upper_queue_buffer, config.link_queue_buffer, average_loss);
  fclose(file); /**< Close the file after writing. */
}

/**
 * @brief Records packet acceptance and dequeue statistics in a CSV file.
 *
 * @details This function:
 *          1. Counts the total number of accepted packets across all tenants
 *          2. Appends both accepted packet count and dequeue count to "record_packet_situation.csv"
 *
 * The function processes an array of packet labels, counting those marked as PACKET_LABEL_ACCEPT,
 * and records this along with the provided dequeue count.
 *
 * @param[in] packets        Pointer to an array of packet labels, where:
 *                          - Array length equals config.tenant_number
 *                          - Each element is a packet label (PACKET_LABEL_ACCEPT or other)
 * @param[in] dequeue_count Total number of packets that were dequeued
 * @param[in] config        Configuration structure containing:
 *                          - data_path: Directory path for output file
 *                          - tenant_number: Total number of tenants (determines packets array size)
 *
 * @pre PACKET_LABEL_ACCEPT must be defined
 * @pre MAX_PATH_LENGTH must be defined
 * @pre data_path directory must exist and be writable
 * @pre packets array must be allocated with size >= config.tenant_number
 * @pre config.tenant_number must be > 0
 *
 * @return void, but will terminate program if:
 *         - File cannot be opened for writing
 *
 * @note Output CSV format:
 *       accepted_packet_count, dequeue_count
 *       where:
 *       - accepted_packet_count: Number of packets marked as PACKET_LABEL_ACCEPT
 *       - dequeue_count: Provided count of dequeued packets
 *
 * @warning Existing file will be appended to, not overwritten
 * @warning Function assumes packets array is properly initialized
 * @warning No bounds checking is performed on the packets array
 * @warning Function does not validate dequeue_count
 *
 * @see PACKET_LABEL_ACCEPT for the acceptance label definition
 */
void record_packet_situation_agrid(int *packets, const int dequeue_count, const configuration config)
{
  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/record_packet_situation.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file"); /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  int packet_number = 0;

  for (int index = 0; index < config.tenant_number; index++)
  {
    if (*(packets + index) == PACKET_LABEL_ACCEPT)
      packet_number += 1;
  }

  // Append the results to the CSV file.
  fprintf(file, "%d, %d\n", packet_number, dequeue_count);
  fclose(file); /**< Close the file after writing. */
}

#endif /* PACKETS_COUNT_H */