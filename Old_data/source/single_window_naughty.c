
// #define REDUCTION
// #define RECORD_PACKETS_GENERATION
// #define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/traffic_generation.h"
#include "../include/GCRA.h"
#include "../include/link_capacity_queue.h"
#include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/simple_V1_naughty.ini"

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    if(argc<3){
        printf("Missing arguments\n");
        return EXIT_FAILURE;
    }

    char command[MAX_COMMAND_LENGTH];
    clock_t execute_clock = clock(); 

    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0){
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    int window = atoi(argv[1]);
    double capacity = atof(argv[2]);

    int tenant_number = config.tenant_number;
    long unit;
    switch (config.unit){
        case UNIT_MBPS:
            unit = MBPS;
            break;
        case UNIT_KBPS:
            unit = KBPS;
            break;
        default:
            unit = MBPS;
    }
    double ratio_normal = (double)(config.mean * unit) / GBPS;
    double ratio_naughty = (double)(config.naughty_mean * unit) / GBPS;
    long step_size = (long)((long)config.packet_size / (GBPS / ONE_SECOND_IN_NS));
    long grid_length = ONE_SECOND_IN_NS/config.packet_size;
    long linkTransmissionInterval = (long)(config.packet_size*(double)ONE_SECOND_IN_NS/(capacity*unit));   //config.packet_size
    // printf("linkTransmissionInterval = %ld\n", linkTransmissionInterval);
    long dequeue_timestamp = 0;
    read_dequeue_timestamp(&dequeue_timestamp, config.data_path);

    link_capacity_queue link;
    read_link_capacity_queue(&link, config.data_path);
    // print_link_queue(link);

    packets_count count;  
    init_Packets_Count(&count, tenant_number, grid_length);
    read_packets_count(&count, config.data_path);
    // print_packets_count(count);

    packets_label label;
    init_Packets_Label(&label, tenant_number, &count);
    read_packets_label(&label, config.data_path);
    // print_packets_label(label);

    GCRA *gcras_1 = initializeGCRAs(tenant_number, config.tau_1, config.packet_size);
    read_gcras(&(gcras_1), tenant_number, config.data_path, 1);

    GCRA *gcras_2 = initializeGCRAs(tenant_number, config.tau_2, config.packet_size);
    read_gcras(&(gcras_2), tenant_number, config.data_path, 2);

    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);
    FILE *file = fopen(filename, "w");
    if (file == NULL){
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }  
    for (long grid = 0; grid < grid_length; grid++){
        int *packets = packet_generation_naughty(grid, ratio_normal, ratio_naughty, config.tenant_number, config.naughty_tenant_number);
        // print_packets(packets, tenant_number);
        long timestamp = window*step_size*grid_length + grid * step_size;
        // printf("timestamp = %-ld\n", timestamp);

        while(dequeue_timestamp <= timestamp){
            dequeue(&link);
            dequeue_timestamp += linkTransmissionInterval;
            // printf("%ld %ld\n", dequeue_timestamp, timestamp);
        }

        for (int tenant = 0; tenant<tenant_number; tenant++){
            // Wheter packet is accepted
            if (*(packets + tenant) == PACKET_LABEL_ACCEPT)
                *(count.count + tenant) += 1;
            else goto RECORD;

            if (*(packets + tenant) == PACKET_LABEL_ACCEPT){
                long rate_1 = (long)(timestamp - (gcras_1 + tenant)->last_time) * (((double)(config.mean + config.standard_deviation) * unit) / ONE_SECOND_IN_NS);
                // printf("rate = %ld\n", rate_1);
                long x = (long)(gcras_1 + tenant)->x - rate_1;
                // printf("rate = %ld\n", x);

                if (x > (gcras_1 + tenant)->tau)
                    *(packets + tenant) = PACKET_LABEL_OVER_UPPERBOUND_DROPPED;
                else{
                    (gcras_1 + tenant)->x = MAX((long)0, x) + (gcras_1 + tenant)->l;
                    (gcras_1 + tenant)->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }
            }else goto RECORD;

            if(*(packets + tenant) == PACKET_LABEL_ACCEPT){
                long rate_2 = (long)(timestamp - (gcras_2 + tenant)->last_time) * (((double)(config.mean) * unit) / ONE_SECOND_IN_NS);
                // printf("rate = %ld\n", rate_2);
                long x = (long)(gcras_2 + tenant)->x - rate_2;
                
                // printf("x = %ld, tau_2 = %ld\n", x, config.tau_2);

                if (x > (gcras_2 + tenant)->tau)
                    *(packets + tenant) = PACKET_LABEL_GCRA_DROPPED;
                else{
                    (gcras_2 + tenant)->x = MAX((long)0, x) + (gcras_2 + tenant)->l;
                    (gcras_2 + tenant)->last_time = timestamp;
                    *(packets + tenant) = PACKET_LABEL_ACCEPT;
                }
            }else goto RECORD;

            if(*(packets + tenant) == PACKET_LABEL_ACCEPT)
                *(packets + tenant) = enqueue(&link);
            else goto RECORD;
            
RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET)
                label.labels[tenant][*(packets + tenant)] += 1;

            if (tenant == tenant_number - 1)
                fprintf(file, "%d\n", *(packets + tenant));
            else
                fprintf(file, "%d, ", *(packets + tenant));
        }
    }
  
    record_packets_count(count, config.data_path);
    record_link_capacity_queue(link, config.data_path);
    record_packets_label(label, config.data_path);
    record_gcras(gcras_1, tenant_number, config.data_path, 1);
    record_gcras(gcras_2, tenant_number, config.data_path, 2);
    record_dequeue_timestamp(dequeue_timestamp, config.data_path);

    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock)/CLOCKS_PER_SEC;
    printf("(%6.4f %), Window = %ld, Time start at %15ld : Execute time : ", 
        (double)window*100.0/config.simulation_time, window, window*step_size*grid_length);
    printf("%f\n", time_taken);

    fclose(file);

    return EXIT_SUCCESS;
}