/**
 * @file packets_count.h
 * @brief Header file for packet counting functions and structures.
 * This file defines the structure and functions related to packet counting for tenants.
 */

#define PACKETS_COUNT_H

/**
* @brief Header for recording regular and naughty packet loss statistics.
* This header defines the format for recording regular and naughty packet losses.
*/
#define RECORD_REGULAR_AND_NAUGHTY_TAU_HEADER "tau,regular_loss,naughty_loss\n"

/**
* @brief Header for recording regular and naughty packet statistics.
* This header defines the format for recording detailed regular and naughty packet statistics.
*/
#define RECORD_REGULAR_AND_NAUGHTY_ALL_HEADER "tau,naughty_mean,naughty_tenant_number,config.state_r,upper_queue_buffer,link_queue_buffer,regular_loss,naughty_loss\n"

/**
* @brief Header for recording average packet loss statistics.
* This header defines the format for recording average packet loss statistics.
*/
#define RECORD_AVERAGE_LOSS_HEADER "tau,naughty_mean,naughty_tenant_number,config.state_r,upper_queue_buffer,link_queue_buffer,average_loss\n"

#ifdef PACKETS_COUNT_H

/** 
* @struct packets_count
* @brief Structure to hold packet count statistics for tenants.
* This structure contains information about the packet count for each tenant,
* including the number of packets, the number of tenants, and the grid length.
*/
typedef struct
{
  long *count;       /**< Array holding the packet count for each tenant. */
  int tenant_number; /**< The number of tenants. */
  long grid_length;  /**< The grid length (total number of events). */
} packets_count;

/**
* @brief Initializes the packet count structure.
*
* This function initializes the `packets_count` structure, allocating memory for the `count` array and 
* setting the tenant number and grid length values.
*
* @param pcount A pointer to the `packets_count` structure to be initialized.
* @param tenant_number The number of tenants.
* @param grid_length The length of the grid (total number of events).
*/
void init_packets_count(packets_count *pcount, int tenant_number, long grid_length)
{
  pcount->count = (long *)calloc(tenant_number, sizeof(long)); /**< Allocate memory for the packet counts. */
  pcount->tenant_number = tenant_number; /**< Set the tenant number. */
  pcount->grid_length = grid_length;     /**< Set the grid length. */
}

/**
* @brief Displays the packet count statistics.
*
* This function prints the packet count for each tenant along with the percentage of total packets 
* for each tenant. It also prints a separator line before and after the statistics.
*
* @param count The `packets_count` structure containing the count and tenant information.
*/
void show_packets_count(packets_count count)
{
  print_equals_line(); /**< Print a separator line. */
  printf("Packet Count =\n");
  
  // Loop through each tenant and display the packet count and percentage.
  for (int tenant = 0; tenant < count.tenant_number; tenant++)
    printf("Number of packets : %-10ld Percentage : %f %\n", *(count.count + tenant), 
          (double)(*(count.count + tenant) * 100) / count.grid_length);
  
  print_equals_line(); /**< Print a separator line. */
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
* This function initializes the `packets_label` structure by allocating memory for the `count` array and the 
* `labels` array. It also copies the packet count values from the provided `packets_count` structure.
*
* @param plabel A pointer to the `packets_label` structure to be initialized.
* @param tenant_number The number of tenants.
* @param pcount A pointer to the `packets_count` structure containing the initial packet counts.
*/
void init_Packets_Label(packets_label *plabel, int tenant_number, packets_count *pcount)
{
  plabel->count = (long *)calloc(tenant_number, sizeof(long)); /**< Allocate memory for the packet counts. */
  memcpy(plabel->count, pcount->count, tenant_number * sizeof(long)); /**< Copy the packet counts from the provided structure. */

  plabel->labels = (int **)malloc(tenant_number * sizeof(int *)); /**< Allocate memory for the labels array. */
  for (int tenant = 0; tenant < tenant_number; tenant++)
    *(plabel->labels + tenant) = (int *)calloc(PACKET_LABEL_TYPE, sizeof(int)); /**< Allocate memory for each tenant's labels. */

  plabel->tenant_number = tenant_number; /**< Set the tenant number. */
}

/**
* @brief Displays the packet label statistics.
*
* This function prints the statistics for each tenant, including the counts for each packet label 
* (ACCEPT, DROP_OVER, DROP_GCRA, CAPACITY) and the corresponding loss percentages. It also computes and 
* displays average statistics, such as the average exceed, average GCRA, and average packet loss.
*
* @param label The `packets_label` structure containing the label and count information.
*/
void show_packets_label(packets_label label)
{
  double average_exceed = 0.0;
  double average_gcra = 0.0;
  double average_loss_p = 0.0;
  double average_loss_a = 0.0;

  print_equals_line(); /**< Print a separator line. */
  printf("label =\n");
  printf("ACCEPT    , DROP_OVER , DROP_LABELED , CAPACITY   : LOSS(PURE),  LOSS\n");

  // Loop through each tenant and display the label counts and loss percentages.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
  {
    printf("%-10d, %-10d, %-10d, %-10d : %-f % , %-f %\n", 
          label.labels[tenant][0], label.labels[tenant][1], label.labels[tenant][2], label.labels[tenant][3],
          (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]),
          (double)(label.labels[tenant][1] + label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][1] + label.labels[tenant][3]));
    
    average_exceed += label.labels[tenant][1]; /**< Sum the exceed labels. */
    average_gcra += label.labels[tenant][2];   /**< Sum the GCRA labels. */
    average_loss_p += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]); /**< Sum the pure loss percentages. */
    average_loss_a += (double)(label.labels[tenant][3] + label.labels[tenant][1]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][1] + label.labels[tenant][3]); /**< Sum the all loss percentages. */
  }

  // Display the average statistics.
  printf(RED_ELOG" average exceed     = %-f\n"RESET, average_exceed / label.tenant_number);
  printf(RED_ELOG" average GCRA       = %-f\n"RESET, average_gcra / label.tenant_number);
  printf(RED_ELOG" average loss(pure) = %-f\n"RESET, average_loss_p / label.tenant_number);
  printf(RED_ELOG" average loss(all)  = %-f\n"RESET, average_loss_a / label.tenant_number);

  print_equals_line(); /**< Print a separator line. */
}
 
/**
 * @brief Prints the regular and naughty tenant statistics.
 *
 * This function calculates and prints the GCRA and packet loss percentages for regular and naughty tenants.
 * It separates the statistics based on the provided `naughty_tenant_number` and prints them individually.
 *
 * @param label The `packets_label` structure containing the packet label statistics for each tenant.
 * @param naughty_tenant_number The number of naughty tenants, used to separate regular and naughty tenants.
 */
void print_regular_and_naughty(packets_label label, int naughty_tenant_number){
  double naughty_GCRA = 0;
  double naughty_Loss = 0;
  double regular_GCRA = 0;
  double regular_Loss = 0;

  // Loop through each tenant and accumulate statistics based on their type (regular or naughty).
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (tenant >= naughty_tenant_number)
    {
      naughty_GCRA += label.labels[tenant][2];  /**< Accumulate GCRA for naughty tenants. */
      naughty_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for naughty tenants. */
    }
    else
    {
      regular_GCRA += label.labels[tenant][2];  /**< Accumulate GCRA for regular tenants. */
      regular_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for regular tenants. */
    }

  // Print the GCRA and loss statistics for regular and naughty tenants.
  printf("regular : GCRA : %-7.12f Loss :%-7.12f% \n", regular_GCRA / (label.tenant_number - naughty_tenant_number), regular_Loss / (label.tenant_number - naughty_tenant_number));
  printf("naughty : GCRA : %-7.12f Loss :%-7.12f% \n", naughty_GCRA / naughty_tenant_number, naughty_Loss / naughty_tenant_number);
}

/**
 * @brief Records the regular and naughty tenant loss statistics in a CSV file.
 *
 * This function calculates the packet loss for regular and naughty tenants, then appends the results to
 * a CSV file named "regular_and_naughty_tau.csv" in the specified `data_path`. It logs the `tau` value, 
 * and the loss percentages for regular and naughty tenants.
 *
 * @param label The `packets_label` structure containing the packet label statistics for each tenant.
 * @param config The configuration structure containing settings like `tau` and `naughty_tenant_number`.
 */
void record_regular_and_naughty_tau(packets_label label, const configuration config) {
  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/regular_and_naughty_tau.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file");  /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double naughty_Loss = 0;
  double regular_Loss = 0;

  // Loop through each tenant and calculate the loss percentage for regular and naughty tenants.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (tenant >= config.naughty_tenant_number)
    {
      naughty_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for naughty tenants. */
    }
    else
    {
      regular_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for regular tenants. */
    }

  // Append the results to the CSV file.
  fprintf(file, "%ld, %f, %f\n", config.tau, regular_Loss, naughty_Loss);
  fclose(file);  /**< Close the file after writing. */
}

/**
 * @brief Records detailed statistics for regular and naughty tenants in a CSV file.
 *
 * This function logs the statistics for regular and naughty tenants into a CSV file named "regular_and_naughty_all.csv".
 * It includes the `tau` value, `naughty_mean`, `naughty_tenant_number`, and other configuration settings, along with
 * the loss percentages for both regular and naughty tenants.
 *
 * @param label The `packets_label` structure containing the packet label statistics for each tenant.
 * @param config The configuration structure containing settings like `tau`, `naughty_mean`, and queue buffers.
 */
void record_regular_and_naughty_all(packets_label label, const configuration config) {
  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/regular_and_naughty_all.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file");  /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double naughty_Loss = 0;
  double regular_Loss = 0;

  // Loop through each tenant and calculate the loss percentage for regular and naughty tenants.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (tenant >= config.naughty_tenant_number)
    {
      naughty_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for naughty tenants. */
    }
    else
    {
      regular_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate loss percentage for regular tenants. */
    }

  // Append the results to the CSV file with additional configuration details.
  fprintf(file, "%ld, %d, %d, %f, %d, %d, %f, %f\n", config.tau, config.naughty_mean, config.naughty_tenant_number, config.state_r, config.upper_queue_buffer, config.link_queue_buffer, regular_Loss, naughty_Loss);
  fclose(file);  /**< Close the file after writing. */
}

/**
 * @brief Records the average loss statistics in a CSV file.
 *
 * This function calculates the average loss across all tenants and appends the results to a CSV file
 * named "record_average_loss.csv". It includes the `tau` value and other configuration settings, along with
 * the calculated average loss for all tenants.
 *
 * @param label The `packets_label` structure containing the packet label statistics for each tenant.
 * @param config The configuration structure containing settings like `tau` and queue buffers.
 */
void record_average_loss(packets_label label, const configuration config){
  char file_path[MAX_PATH_LENGTH];
  sprintf(file_path, "%s/record_average_loss.csv", config.data_path);

  FILE *file = fopen(file_path, "a");
  if (!file)
  {
    perror("Error opening file");  /**< Handle file open errors. */
    exit(EXIT_FAILURE);
  }

  double average_loss = 0.0;

  // Loop through each tenant and calculate the average loss.
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
  {
    average_loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]);  /**< Calculate average loss percentage. */
  }

  // Append the results to the CSV file.
  fprintf(file, "%ld, %d, %d, %f, %d, %d, %f\n", config.tau, config.naughty_mean, config.naughty_tenant_number, config.state_r, config.upper_queue_buffer, config.link_queue_buffer, average_loss);
  fclose(file);  /**< Close the file after writing. */
}

#endif /* PACKETS_COUNT_H */