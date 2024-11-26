#define PACKETS_COUNT_H 
#ifdef PACKETS_COUNT_H 

typedef struct{
  long *count;
  int tenant_number;
  long grid_length;
} packets_count;

typedef struct{
  long *count;
  int tenant_number;
  long grid_length;
} packets_label;

void init_Packets_Count(packets_count* pcount, int tenant_number, long grid_length){
  pcount->count = (long *)calloc(tenant_number, sizeof(long));
  pcount->tenant_number = tenant_number;
  pcount->grid_length = grid_length;
}

void print_packets_count(packets_count count){
  // printf("grid length = %ld\n", count.grid_length);
  print_equals_line();
  printf("Packet Count =\n");
  for (int tenant = 0; tenant < count.tenant_number; tenant++)
      printf("Number of packets : %-10ld Percentage : %f %\n", *(count.count + tenant), (double)(*(count.count + tenant) * 100) / count.grid_length);
  print_equals_line();
}

void record_packets_count(packets_count count, const char* folder_path){
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

void read_packets_count(packets_count* pcount, const char* folder_path){
  char data_path[MAX_PATH_LENGTH];
  sprintf(data_path, "%s/count.csv", folder_path);

  FILE* file = fopen(data_path, "r");
  if (!file) {
      perror("Error opening file");
      return;
  }

  char line[MAX_BUFFER_SIZE];
  size_t index = 0;

  // Read each line from the CSV
  while (fgets(line, sizeof(line), file) && index < pcount->tenant_number) {
      int tenant;
      int packet;
      float percentage;

      // Parse the line for tenant, packet, and percentage
      if (sscanf(line, "%d, %d, %f", &tenant, &packet, &percentage) == 3) {
          pcount->count[index++] = packet; // Store the packet value
      } else {
          fprintf(stderr, "Error parsing line: %s\n", line);
      }
  }

  fclose(file);

  // Ensure we read exactly `length` rows
  if (index != pcount->tenant_number) {
      fprintf(stderr, "Warning: Expected %zu rows but found %zu rows\n", pcount->tenant_number, index);
  }
}
#endif