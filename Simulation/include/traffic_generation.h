#include <math.h>
#include "../include/tenant.h"

#define PYTHON_PACKET_GENERATION_PATH "../python/packet_generation.py"
#define EXTERNAL_SOURCE_PATH "traffic_generation_tenant"

#define TRAFFIC_GENERATION_H

#ifdef TRAFFIC_GENERATION_H

    void packet_generation_interval(tenant* ptenant, int mean, int standard_deviation){
        if(ptenant->simulation_time <= 0){
            printf("Error non-positive simulation time of tenant[%d]\n", ptenant->identifier);
            exit(EXIT_FAILURE);
        }

        srand((unsigned int)time(NULL) + ptenant->identifier);
        long long current_sum = 0;
        for (long index=0;index<ptenant->simulation_time;index++) {
            *(ptenant->generated_packets+index) = (mean - standard_deviation) +rand()%(2*standard_deviation+1);
            current_sum += *(ptenant->generated_packets+index);
        }

        long long current_average = (long long)(current_sum/ptenant->simulation_time);
        long long difference = mean*ptenant->simulation_time - current_sum;

        for (long index=0;index<ptenant->simulation_time;index++) {

            if (difference == 0) break;

            int adjustment = difference > 0 ? 1 : -1;
            int new_value = *(ptenant->generated_packets+index) + adjustment;

            // Ensure that the new value is still within the bounds
            if (new_value >= (mean - standard_deviation) && new_value <= (mean + standard_deviation)) {
                *(ptenant->generated_packets+index) = new_value;
                difference -= adjustment;
            }
        }
    }

    double generate_gaussian(double mean, double standard_deviation) {
        static double spare;
        static int hasSpare = 0;

        if (hasSpare) {
            hasSpare = 0;
            return mean + standard_deviation*spare;
        } else {
            double u, v, s;
            do {
                u = (rand() / (double)RAND_MAX) * 2.0 - 1.0;  
                v = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
                s = u * u + v * v;
            } while (s >= 1.0 || s == 0);  

            s = sqrt(-2.0 * log(s) / s);  

            spare = v * s;  
            hasSpare = 1;

            return mean + standard_deviation * (u * s);  
        }
    }

    void packet_generation_gaussian(tenant* ptenant, int mean, int standard_deviation){
        srand((unsigned int)time(NULL) + ptenant->identifier);

        double current_sum = 0;
        for (long index=0;index<ptenant->simulation_time;index++) {
            *(ptenant->generated_packets+index) = (int)round(generate_gaussian(mean, standard_deviation));
            current_sum += *(ptenant->generated_packets+index);
        }

        double desired_sum = mean * ptenant->simulation_time;
        double difference = desired_sum - current_sum;

        for (long index=0;index<ptenant->simulation_time && difference != 0;index++) {
            if (difference > 0) {
                *(ptenant->generated_packets+index) += 1;
                difference -= 1;
            } else if (difference < 0) {
                *(ptenant->generated_packets+index) -= 1;
                difference += 1;
            }
        }
    }

    void packet_generation_all_naughty(tenant* ptenant, int naughty_mean, int naughty_standard_deviation){
        if(ptenant->simulation_time <= 0){
            printf("Error non-positive simulation time of tenant[%d]\n", ptenant->identifier);
            exit(EXIT_FAILURE);
        }

        srand((unsigned int)time(NULL) + ptenant->identifier);
        long long current_sum = 0;
        for (long index=0;index<ptenant->simulation_time;index++) {
            *(ptenant->generated_packets+index) = (naughty_mean - naughty_standard_deviation) +rand()%(2*naughty_standard_deviation+1);
            current_sum += *(ptenant->generated_packets+index);
        }

        long long current_average = (long long)(current_sum/ptenant->simulation_time);
        long long difference = naughty_mean*ptenant->simulation_time - current_sum;

        for (long index=0;index<ptenant->simulation_time;index++) {

            if (difference == 0) break;

            int adjustment = difference > 0 ? 1 : -1;
            int new_value = *(ptenant->generated_packets+index) + adjustment;

            // Ensure that the new value is still within the bounds
            if (new_value >= (naughty_mean - naughty_standard_deviation) && new_value <= (naughty_mean + naughty_standard_deviation)) {
                *(ptenant->generated_packets+index) = new_value;
                difference -= adjustment;
            }
        }
    }

    void tenant_packet_generation(tenant* ptenant, int mean, int standard_deviation, int naughty_mean, int naughty_standard_deviation){
        switch(ptenant->traffic_mode){
            case TENANT_TRAFFIC_MODE_INTERVAL:
                packet_generation_interval(ptenant, mean, standard_deviation);
                break;
            case TENANT_TRAFFIC_MODE_GAUSSIAN:
                packet_generation_gaussian(ptenant, mean, standard_deviation);
                break;
            case TENANT_TRAFFIC_MODE_ALL_NAUGHTY:
                packet_generation_all_naughty(ptenant, naughty_mean, naughty_standard_deviation);
                break;
            default:
                packet_generation_interval(ptenant, mean, standard_deviation);
                break;
        }
    }
    
    void tenants_packet_generation(tenant* tenants, int tenant_number,int mean, int standard_deviation, int naughty_mean, int naughty_standard_deviation){
        for(int index=0;index<tenant_number;index++)
            tenant_packet_generation((tenant*)(tenants+index), mean, standard_deviation, naughty_mean, naughty_standard_deviation);
    }

    void translate_band_to_packets(tenant* ptenant, long long unit, int packet_size){
      for (long index=0;index<ptenant->simulation_time;index++){
        long long band = (long long)(*(ptenant->generated_packets+index));
        // long long packets_number = (long long)(band*unit)/(long long)(packet_size);
        long long packets_number = (long long)(band*unit)/(long long)packet_size;
        *(ptenant->generated_packets+index) = packets_number;
      }
    } 

    void translate_tenants_band_to_packets(tenant* tenants, int tenant_number, long long unit, int packet_size){
      for(int index=0;index<tenant_number;index++)
        translate_band_to_packets((tenant*)(tenants+index), unit, packet_size);
    }

    void print_generated_packets(tenant t){
        long long sum = 0;
        for(long long time_s=0;time_s<t.simulation_time;time_s++){
          printf("tenant [%-3d] %-9ld : %ld\n", t.identifier, time_s, t.generated_packets[time_s]);
          sum += t.generated_packets[time_s];
        }

        printf("tenant [%-3d] generates [%ld] packets with mean [%f].\n", t.identifier, sum, (double)sum/(double)t.simulation_time);
        // print_generated_packets((tenant)(*tenants));
    }

    void record_generated_packets(tenant* ptenant, const char* folder_path){
        char filename[MAX_PATH_LENGTH];
        sprintf(filename, "%s/tenant_%d.csv", folder_path, ptenant->identifier);

        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            printf("Failed to open file %s for writing.\n", filename);
            exit(EXIT_FAILURE);  
        }

        fprintf(file, "time_s, packets number per second\n"); 
        for (int index = 0; index < ptenant->simulation_time; index++) {
            fprintf(file, "%d,%ld\n", index, *(ptenant->generated_packets+index));  
        }

        fclose(file);

        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "python3 "PYTHON_PACKET_GENERATION_PATH" 0 %s %d", folder_path, ptenant->identifier);
        system(command);     
        // record_generated_packets((tenant*)(tenants), STORED_PACKET_GENERATION_PATH);   
    }

    void record_tenants_generated_packets(tenant* tenants, int tenant_number, const char* folder_path){
        for(int index=0;index<tenant_number;index++)
            record_generated_packets((tenant*)(tenants+index), folder_path);
    }

    void timestamp_translation(tenant* ptenant){
        long long packet_number = 0;
        for (long index=0;index<ptenant->simulation_time;index++)
            packet_number += (long long)*(ptenant->generated_packets+index);
        ptenant->packet_number = packet_number;
        // printf("%ld\n", packet_number);

        if(ptenant->packet_number == 0){
            printf("Error : tenant[%d] has 0 packet number!\n", ptenant->identifier);
            exit(EXIT_FAILURE);
        }

        ptenant->traffic_timestamp = (long long*)malloc(sizeof(long long)*packet_number);
        ptenant->packet_label = (int*)calloc(packet_number, sizeof(int));

        long long time_index = 0;
        long long timestamp = 0;
        for (long index=0;index<ptenant->simulation_time;index++){
            for(long long jndex=0;jndex<*(ptenant->generated_packets+index);jndex++){
                timestamp = (long long)(timestamp + ONE_SECOND_IN_NS/((long long)*(ptenant->generated_packets+index)));
                // printf("%ld %ld\n", time_index, *(ptenant->traffic_timestamp + time_index));
                *(ptenant->traffic_timestamp + time_index) = timestamp;
                time_index += 1;
            }
        }

        // timestamp_translation((tenant*)(tenants));
    }

    void tenants_timestamp_translation(tenant* tenants, int tenant_number){
        for(int index=0;index<tenant_number;index++){
            timestamp_translation((tenant*)(tenants+index));
            printf("%d\n", index);
        }
    }

    void timestamp_translation_external(const char* configuration_path, unsigned int identifier, const char* record_packets_generation_path, const char* record_timestamp_path){
        system("gcc ./"EXTERNAL_SOURCE_PATH".c inih/ini.c -o ../execution/"EXTERNAL_SOURCE_PATH" -lm");
        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "../execution/"EXTERNAL_SOURCE_PATH" %s %d %s %s", configuration_path, identifier, record_packets_generation_path, record_timestamp_path);
        system(command);        
    }

    void tenants_timestamp_translation_external(const char* configuration_path, int tenant_number, const char* record_packets_generation_path, const char* record_timestamp_path){
        system("gcc ./"EXTERNAL_SOURCE_PATH".c inih/ini.c -o ../execution/"EXTERNAL_SOURCE_PATH" -lm");
        char command[MAX_COMMAND_LENGTH];
        for(int index = 0; index < tenant_number; index++){
            sprintf(command, "../execution/"EXTERNAL_SOURCE_PATH" %s %d %s %s", configuration_path, index, record_packets_generation_path, record_timestamp_path);
            system(command); 
        }
    }

    size_t count_lines(FILE *file) {
        size_t lines = 0;
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            lines++;
        }
        rewind(file); // Reset file pointer to the beginning
        return lines - 1; // Subtract 1 for the header line
    }

    long long *read_csv(const char *filename, size_t *size) {
        FILE *file = fopen(filename, "r");
        if (!file) {
            perror("Failed to open file");
            return NULL;
        }

        // Count lines and allocate memory for the array
        size_t line_count = count_lines(file);
        *size = line_count;
        long long *timestamps = malloc(line_count * sizeof(long long));
        if (!timestamps) {
            perror("Failed to allocate memory");
            fclose(file);
            return NULL;
        }

        // Skip the header line
        char buffer[1024];
        fgets(buffer, sizeof(buffer), file);

        // Read the file line by line
        size_t index = 0;
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            char *token = strtok(buffer, ","); // Split on comma
            token = strtok(NULL, ","); // Get the second column (timestamp)
            if (token) {
                timestamps[index++] = atol(token);
            }
        }

        fclose(file);
        return timestamps;
    }

    void print_timestamps(tenant* ptenant){
        for(long index=0;index<ptenant->packet_number;index++)
            printf("tenant [%-3d] %-9ld : %-ld\n", ptenant->identifier, index, ptenant->traffic_timestamp[index]);
    }

    void record_timestamps(tenant* ptenant, const char* folder_path){
        char filename[MAX_PATH_LENGTH];
        sprintf(filename, "%s/tenant_%d.csv", folder_path, ptenant->identifier);

        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            printf("Failed to open file %s for writing.\n", filename);
            exit(EXIT_FAILURE);  
        }

        fprintf(file, "time_s, timestamps serie of packets\n"); 
        for (int index = 0; index < ptenant->packet_number; index++) {
            fprintf(file, "%d,%ld\n", index, ptenant->traffic_timestamp[index]);  
        }

        fclose(file);  
    }

    void record_tenants_timestamps(tenant* tenants, int tenant_number, const char* folder_path){
        for(int index=0;index<tenant_number;index++)
            record_timestamps((tenant*)(tenants+index), folder_path);
    }

    void print_packets_label(tenant* ptenant){
        for (long index = 0; index < ptenant->packet_number; index++)
            printf("tenant [%-3d] %-9ld : %-d\n", ptenant->identifier, index, ptenant->packet_label[index]);
    }

    void record_packets_label(tenant* ptenant, const char* folder_path){
        char filename[MAX_PATH_LENGTH];
        sprintf(filename, "%s/tenant_%d.csv", folder_path, ptenant->identifier);

        FILE* file = fopen(filename, "w");
        if (file == NULL) {
            printf("Failed to open file %s for writing.\n", filename);
            exit(EXIT_FAILURE);  
        }

        long long sum = 0;
        fprintf(file, "timestamps, packet label\n"); 
        for (long index = 0; index < ptenant->packet_number; index++){
            fprintf(file ,"%ld, %ld\n", index, ptenant->packet_label[index]);
            sum+=ptenant->packet_label[index];
        }

        // printf("sum = %ld, %ld  %ld\n", sum, ptenant->packet_number, 161*3000);
            

        fclose(file); 
    }
#endif