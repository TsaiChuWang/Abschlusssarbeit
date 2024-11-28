
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

#define CONFIGURATION_PATH "../configuration/simple_V1.ini"

#define GENERATE_TRAFFIC 1

int main(int argc, char *argv[])
{
    if(argc<4){
        printf("Missing arguments\n");
        return EXIT_FAILURE;
    }

    char command[MAX_COMMAND_LENGTH];
    clock_t execute_clock = clock(); 

    int window = atoi(argv[1]);
    double capacity = atof(argv[2]);
    char configuration_path[MAX_PATH_LENGTH];
    strcpy(configuration_path, argv[3]);

    configuration config;

    if (ini_parse(configuration_path, handler, &config) < 0){
        printf("Can't load configuration \"%s\"\n", configuration_path);
        return EXIT_FAILURE;
    }

    // printf("data path = %s\n", config.data_path);

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
    double ratio = (double)(config.mean * unit) / GBPS;
    long step_size = (long)((long)config.packet_size / (GBPS / ONE_SECOND_IN_NS));
    // long grid_length = ONE_SECOND_IN_NS/config.packet_size;
    long grid_length = 2000;
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

#ifdef RECORD_EACH_GRID
    char filename[MAX_PATH_LENGTH];
    sprintf(filename, "%s/packets.csv", config.data_path);
    FILE *file = fopen(filename, "w");
    if (file == NULL){
        printf("Failed to open file %s for writing.\n", filename);
        exit(EXIT_FAILURE);
    }  
#endif

    for (long grid = 0; grid < grid_length; grid++){
        int *packets = packet_generation_uniform(grid, ratio, tenant_number);
        // print_packets(packets, tenant_number);
        long timestamp = window*step_size*grid_length + grid * step_size;
        // printf("timestamp = %-ld\n", timestamp);

        int dequeue_count = 0;
        int enqueue_count = 0;
        int enqueue_full = 0;

        while(dequeue_timestamp <= timestamp){
            dequeue(&link);
            dequeue_timestamp += linkTransmissionInterval;
            dequeue_count ++;
            // printf("%ld %ld %d\n", dequeue_timestamp, timestamp, link.front);
        }

        // if(link.front >0 || link.rear>0)
            // printf("link :front = %d, rear = %d\n", link.front, link.rear);

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

            if(*(packets + tenant) == PACKET_LABEL_ACCEPT){
                *(packets + tenant) = enqueue(&link);
                enqueue_count ++;
                if(*(packets + tenant) == PACKET_LABEL_OVER_CAPACITY_DROPPED)
                    enqueue_full ++;
            }
                
            else goto RECORD;
            
RECORD:
            if (*(packets + tenant) != PACKET_LABEL_NO_PACKET){
                // printf("%d\n", *(packets + tenant));
                label.labels[tenant][*(packets + tenant)] += 1;
            }
                
#ifdef RECORD_EACH_GRID
            if (tenant == tenant_number - 1)
                fprintf(file, "%d\n", *(packets + tenant));
            else
                fprintf(file, "%d, ", *(packets + tenant));
#endif
            
        }

        // if(enqueue_full>0)
        //     printf("link :front = %3d, rear = %3d dequeue_count = %3d, enqueue_count = %3d, enqueue_full = %3d Grid[%4d](%ld)\n", link.front, link.rear, dequeue_count, enqueue_count, enqueue_full, grid, timestamp);
        // printf("link :front = %3d, rear = %3d dequeue_count = %3d, enqueue_count = %3d, enqueue_full = %3d\n", link.front, link.rear, dequeue_count, enqueue_count, enqueue_full);
    }

    record_packets_count(count, config.data_path);
    record_link_capacity_queue(link, config.data_path);
    record_packets_label(label, config.data_path);
    // print_packets_label(label);
    record_gcras(gcras_1, tenant_number, config.data_path, 1);
    record_gcras(gcras_2, tenant_number, config.data_path, 2);
    record_dequeue_timestamp(dequeue_timestamp, config.data_path);

    execute_clock = clock() - execute_clock;
    double time_taken = ((double)execute_clock)/CLOCKS_PER_SEC;
    printf("(%6.4f %), Window = %ld, Time start at %15ld : Execute time : ", 
        (double)window*100.0/config.simulation_time, window, window*step_size*grid_length);
    printf("%f\n", time_taken);

#ifdef RECORD_EACH_GRID
    fclose(file);
#endif
    return EXIT_SUCCESS;
}