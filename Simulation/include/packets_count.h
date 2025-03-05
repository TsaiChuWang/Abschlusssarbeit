#define PACKETS_COUNT_H
#ifdef PACKETS_COUNT_H

typedef struct
{
  long *count;
  int tenant_number;
  long grid_length;
} packets_count;

typedef struct
{
  int **labels;
  long *count;
  int tenant_number;
} packets_label;

void init_Packets_Count(packets_count *pcount, int tenant_number, long grid_length)
{
  pcount->count = (long *)calloc(tenant_number, sizeof(long));
  pcount->tenant_number = tenant_number;
  pcount->grid_length = grid_length;
}

void print_packets_count(packets_count count)
{
  // printf("grid length = %ld\n", count.grid_length);
  print_equals_line();
  printf("Packet Count =\n");
  for (int tenant = 0; tenant < count.tenant_number; tenant++)
    printf("Number of packets : %-10ld Percentage : %f %\n", *(count.count + tenant), (double)(*(count.count + tenant) * 100) / count.grid_length);
  print_equals_line();
}

void record_packets_count(packets_count count, const char *folder_path)
{
  char data_path[MAX_PATH_LENGTH];
  sprintf(data_path, "%s/count.csv", folder_path);

  FILE *file = fopen(data_path, "w");
  if (file == NULL)
  {
    printf("Failed to open file %s for writing.\n", data_path);
    exit(EXIT_FAILURE);
  }

  // fprintf(file, "tenant, packets, percentage\n");
  for (int tenant = 0; tenant < count.tenant_number; tenant++)
    fprintf(file, "%d, %ld, %f\n", tenant, *(count.count + tenant), (double)(*(count.count + tenant) * 100) / count.grid_length);
  fclose(file);
}

void read_packets_count(packets_count *pcount, const char *folder_path)
{
  char data_path[MAX_PATH_LENGTH];
  sprintf(data_path, "%s/count.csv", folder_path);

  FILE *file = fopen(data_path, "r");
  if (!file)
  {
    perror("Error opening file");
    return;
  }

  char line[MAX_BUFFER_SIZE];
  size_t index = 0;

  // Read each line from the CSV
  while (fgets(line, sizeof(line), file) && index < pcount->tenant_number)
  {
    int tenant;
    int packet;
    float percentage;

    // Parse the line for tenant, packet, and percentage
    if (sscanf(line, "%d, %d, %f", &tenant, &packet, &percentage) == 3)
    {
      pcount->count[index++] = packet; // Store the packet value
    }
    else
    {
      fprintf(stderr, "Error parsing line: %s\n", line);
    }
  }

  fclose(file);

  // Ensure we read exactly `length` rows
  if (index != pcount->tenant_number)
  {
    fprintf(stderr, "Warning: Expected %zu rows but found %zu rows\n", pcount->tenant_number, index);
  }
}

void init_Packets_Label(packets_label *plabel, int tenant_number, packets_count *pcount)
{
  plabel->count = (long *)calloc(tenant_number, sizeof(long));
  memcpy(plabel->count, pcount->count, tenant_number * sizeof(long));

  plabel->labels = (int **)malloc(tenant_number * sizeof(int *));
  for (int tenant = 0; tenant < tenant_number; tenant++)
    *(plabel->labels + tenant) = (int *)calloc(PACKET_LABEL_TYPE, sizeof(int));

  plabel->tenant_number = tenant_number;
}

void print_packets_label(packets_label label)
{
  double average_loss = 0.0;
  print_equals_line();
  printf("label =\n");
  printf("ACCEPT    , DROP_OVER , DROP_GCRA , CAPACITY   : LOSS(PURE),  LOSS\n");
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
  {
    printf("%-10d, %-10d, %-10d, %-10d : %-f % , %-f %\n", label.labels[tenant][0], label.labels[tenant][1], label.labels[tenant][2], label.labels[tenant][3],
           (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3]),
           (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]));
    average_loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]);
  }
  printf("\x1B[1;31m average loss = %-f\x1B[0m\n", average_loss / label.tenant_number);
  print_equals_line();
}

void record_packets_label(packets_label label, const char *folder_path)
{
  char data_path[MAX_PATH_LENGTH];
  sprintf(data_path, "%s/label.csv", folder_path);

  FILE *file = fopen(data_path, "w");
  if (file == NULL)
  {
    printf("Failed to open file %s for writing.\n", data_path);
    exit(EXIT_FAILURE);
  }

  // fprintf(file, "ACCEPT, DROP_OVER, DROP_GCRA, CAPACITY, LOSS(PURE),  LOSS\n");
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    fprintf(file, "%-10d, %-10d, %-10d, %-10d, %-f, %-f \n", label.labels[tenant][0], label.labels[tenant][1], label.labels[tenant][2], label.labels[tenant][3],
            (double)(label.labels[tenant][2]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][2] + label.labels[tenant][3]),
            (double)(label.labels[tenant][2] + label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][2] + label.labels[tenant][3] + label.labels[tenant][4]));
  fclose(file);
}

void read_packets_label(packets_label *plabel, const char *folder_path)
{
  char data_path[MAX_PATH_LENGTH];
  sprintf(data_path, "%s/label.csv", folder_path);

  FILE *file = fopen(data_path, "r");
  if (!file)
  {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Read and parse the file line by line
  char line[MAX_BUFFER_SIZE];
  size_t row = 0;
  while (fgets(line, sizeof(line), file) && row < plabel->tenant_number)
  {
    int temp_label[PACKET_LABEL_TYPE];
    double temp_percentage[2];
    if (sscanf(line, "%ld , %ld , %ld , %ld , %f , %f",
               &temp_label[0], &temp_label[1], &temp_label[2], &temp_label[3], &temp_percentage[0], &temp_percentage[1]) == (PACKET_LABEL_TYPE + 2))
    {
      for (size_t col = 0; col < PACKET_LABEL_TYPE; col++)
        plabel->labels[row][col] = (int)temp_label[col];

      row++;
    }
    else
    {
      fprintf(stderr, "Error parsing line: %s\n", line);
    }
  }

  fclose(file);
}

void print_naughty(packets_label label, int naughty_tenant_number)
{
  FILE *file = fopen("../data/naughty.csv", "a");
  if (!file)
  {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  double naughty_GCRA = 0;
  double naughty_Loss = 0;
  double regular_GCRA = 0;
  double regular_Loss = 0;
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (tenant >= naughty_tenant_number)
    {
      naughty_GCRA += label.labels[tenant][2];
      naughty_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]);
    }
    else
    {

      regular_GCRA += label.labels[tenant][2];
      regular_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]);
    }

  printf("regular : GCRA : %-7f Loss :%-7f% \n", regular_GCRA / (label.tenant_number - naughty_tenant_number), regular_Loss / (label.tenant_number - naughty_tenant_number));

  printf("naughty : GCRA : %-7f Loss :%-7f% \n", naughty_GCRA / naughty_tenant_number, naughty_Loss / naughty_tenant_number);
  fprintf(file, "%f, %f\n", regular_Loss / (label.tenant_number - naughty_tenant_number), naughty_Loss / naughty_tenant_number);
  fclose(file);
}

void calculate_tau_variation(packets_label label, int naughty_tenant_number)
{
  FILE *file = fopen("../data/tau_variation.csv", "a");
  if (!file)
  {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  double naughty_GCRA = 0;
  double naughty_Loss = 0;
  double regular_GCRA = 0;
  double regular_Loss = 0;
  for (int tenant = 0; tenant < label.tenant_number; tenant++)
    if (tenant >= naughty_tenant_number)
    {
      naughty_GCRA += label.labels[tenant][2];
      naughty_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]);
    }
    else
    {

      regular_GCRA += label.labels[tenant][2];
      regular_Loss += (double)(label.labels[tenant][3]) * 100.0 / (label.labels[tenant][0] + label.labels[tenant][3] + label.labels[tenant][4]);
    }

  printf("regular : GCRA : %-7f Loss :%-7f% \n", regular_GCRA / (label.tenant_number - naughty_tenant_number), regular_Loss / (label.tenant_number - naughty_tenant_number));

  printf("naughty : GCRA : %-7f Loss :%-7f% \n", naughty_GCRA / naughty_tenant_number, naughty_Loss / naughty_tenant_number);
  fprintf(file, "%f, %f\n", regular_Loss / (label.tenant_number - naughty_tenant_number), naughty_Loss / naughty_tenant_number);
  fclose(file);
}
#endif