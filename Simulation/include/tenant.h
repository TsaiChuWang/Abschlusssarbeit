
#define TENANT_H

#define TENANT_TRAFFIC_MODE_INTERVAL 0
#define TENANT_TRAFFIC_MODE_GAUSSIAN 1
#define TENANT_TRAFFIC_MODE_ALL_NAUGHTY 2

#ifdef TENANT_H

    typedef struct {
        unsigned int identifier;
        int traffic_mode;

        long simulation_time;
        long long* generated_packets;

        long long packet_number;
        long long* traffic_timestamp;
        int* packet_label;

    } tenant;

    tenant initialize_tenant(unsigned int identifier, long simulation_time){
        tenant t;
        t.identifier = identifier;
        t.traffic_mode = TENANT_TRAFFIC_MODE_INTERVAL;

        t.simulation_time = simulation_time;
        t.generated_packets = (long long*)malloc(sizeof(long long)*simulation_time);

        t.packet_number = 0;
        t.traffic_timestamp = NULL;
        t.packet_label = NULL;
        return t;
    }

    tenant* initialize_tenants(int tenant_number, long simulation_time){
        unsigned int identifier = 0;
        tenant* tenants = (tenant*)malloc(sizeof(tenant)*tenant_number);
        for(int index = 0;index<tenant_number;index++){
            *(tenants+index) = initialize_tenant(identifier, simulation_time);
            identifier++;
        }
        return tenants;
    }

#endif

    // packet_generation_interval((tenant*)(tenants), config.mean, config.standard_deviatio);
    // packet_generation_gaussian((tenant*)(tenants), config.mean, config.standard_deviatio);
    // packet_generation_all_naughty((tenant*)(tenants), config.naughty_mean, config.naughty_standard_deviation);
    // tenant_packet_generation((tenant*)(tenants), config.mean, config.standard_deviation, config.naughty_mean, config.naughty_standard_deviation);
    // print_generated_packets(*(tenants));
    // record_generated_packets((tenant*)(tenants), RECORD_PACKET_GENERATION_PATH);

    // timestamp_translation((tenant*)(tenants));
    // print_timestamps((tenant*)(tenants));