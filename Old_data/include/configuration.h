
#include <string.h>

#define CONFIG_H

#define TRAFFIC_MODE_INTERVAL 0
#define TRAFFIC_MODE_GAUSSIAN 1
#define TRAFFIC_MODE_ALL_NAUGHTY 2

#define UNIT_MBPS 0
#define UNIT_KBPS 1

#ifdef REDUCTION
    #define INITIAL_CONFIGURATION_TENANT_NUMBER 100
    #define INITIAL_CONFIGURATION_SIMULATION_simulation_time 1
    #define INITIAL_CONFIGURATION_ERROR 0.001
    #define INITIAL_CONFIGURATION_UNIT 0

    #define INITIAL_CONFIGURATION_INPUT_RATE (long)1073741824
    #define INITIAL_CONFIGURATION_TRAFFIC_MODE 0
    #define INITIAL_CONFIGURATION_MEAN 120
    #define INITIAL_CONFIGURATION_STANDARD_DEVIATION 40
    #define INITIAL_CONFIGURATION_PACKET_SIZE 512

    #define INITIAL_CONFIGURATION_NAUGHTY_MEAN 150
    #define INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION 10
    #define INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER 50

    #define INITIAL_CONFIGURATION_BUCKET_DEPTH_1 4096
    #define INITIAL_CONFIGURATION_BUCKET_DEPTH_2 7513

    void reduction_inif_file(const char* filename){
        FILE* file = fopen(filename, "w");
        if (!file) {
            printf("Unable to open file  \"%s\" for writing\n", filename);
            perror("Unable to open file for writing");  
            return;
        }

        fprintf(file, "[simulation]\n");
        fprintf(file, "tenant_number = %d\n", INITIAL_CONFIGURATION_TENANT_NUMBER);  
        fprintf(file, "simulation_time = %ld\n", INITIAL_CONFIGURATION_SIMULATION_simulation_time);  
        fprintf(file, "error = %f\n", INITIAL_CONFIGURATION_ERROR);  
        fprintf(file, "data_path = ../data/test\n");  
        fprintf(file, "unit = %d\n", INITIAL_CONFIGURATION_UNIT);

        // Write the [traffic] section
        fprintf(file, "[traffic]\n");
        fprintf(file, "input_rate = %ld\n", INITIAL_CONFIGURATION_INPUT_RATE);  
        fprintf(file, "traffic_mode = %d\n", INITIAL_CONFIGURATION_TRAFFIC_MODE);  
        fprintf(file, "mean = %d\n", INITIAL_CONFIGURATION_MEAN); 
        fprintf(file, "standard_deviation = %d\n", INITIAL_CONFIGURATION_STANDARD_DEVIATION);  
        fprintf(file, "packet_size = %d\n", INITIAL_CONFIGURATION_PACKET_SIZE);  

        fprintf(file, "naughty_mean = %d\n", INITIAL_CONFIGURATION_NAUGHTY_MEAN);  
        fprintf(file, "naughty_standard_deviation = %d\n", INITIAL_CONFIGURATION_NAUGHTY_STANDARD_DEVIATION);  
        fprintf(file, "naughty_tenant_number = %d\n", INITIAL_CONFIGURATION_NAUGHTY_TENANT_NUMBER);  

        // Write the [GCRA] section
        fprintf(file, "[GCRA]\n");
        fprintf(file, "tau_1 = %ld\n", INITIAL_CONFIGURATION_BUCKET_DEPTH_1);  
        fprintf(file, "tau_2 = %ld\n", INITIAL_CONFIGURATION_BUCKET_DEPTH_2); 

        // Close the file
        fclose(file); 
    }

#endif

#ifdef CONFIG_H

    typedef struct {
        int     tenant_number;  
        long    simulation_time;  
        double  error;      
        char*   data_path;
        int     unit;

        long    input_rate;
        int     traffic_mode;           
        int     mean;               
        int     standard_deviation; 
        int     packet_size;

        int     naughty_mean;               
        int     naughty_standard_deviation; 
        int     naughty_tenant_number;     
 
        long tau_1;
        long tau_2;
    } configuration;

    static int handler(void* config, const char* section, const char* name, const char* value){
        configuration* pconfig = (configuration*)config;  
    
#define MATCH(section_, name_) (strcmp(section, section_) == 0 && strcmp(name, name_) == 0) 
     
        if (MATCH("simulation", "tenant_number")) 
            pconfig->tenant_number = atoi(value);       
        else if (MATCH("simulation", "simulation_time")) 
            pconfig->simulation_time = (long)atoi(value); 
        else if (MATCH("simulation", "error")) 
            pconfig->error = atof(value);              
        else if (MATCH("simulation", "data_path")) {
            char* temp_str = malloc(strlen(value) + 1);  
            strcpy(temp_str, value);  
            pconfig->data_path = temp_str;  
        }
        else if (MATCH("simulation", "unit")) 
            pconfig->unit = atoi(value);              

        else if (MATCH("traffic", "input_rate")) 
            pconfig->input_rate = strtol(value, NULL, 10);   
        else if (MATCH("traffic", "traffic_mode")) 
            pconfig->traffic_mode = atoi(value);                    
        else if (MATCH("traffic", "mean")) 
            pconfig->mean = atoi(value);                        
        else if (MATCH("traffic", "standard_deviation")) 
            pconfig->standard_deviation = atoi(value);   
        else if (MATCH("traffic", "packet_size")) 
            pconfig->packet_size = atoi(value);   

        else if (MATCH("traffic", "naughty_mean")) 
            pconfig->naughty_mean = atoi(value);                
        else if (MATCH("traffic", "naughty_standard_deviation")) 
            pconfig->naughty_standard_deviation = atoi(value);  
        else if (MATCH("traffic", "naughty_tenant_number")) 
            pconfig->naughty_tenant_number = atoi(value);       
        
        else if (MATCH("GCRA", "tau_1")) 
            pconfig->tau_1 = strtol(value, NULL, 10);  
        else if (MATCH("GCRA", "tau_2")) 
            pconfig->tau_2 = strtol(value, NULL, 10);  

        return SUCCESS; 
    }

    double obtain_capacity() {
        FILE *file;      
        double capacity;    

        file = fopen(CAPACITY_DATA_PATH, "r");
        if (file == NULL) {
            printf("Cannot read file %s\n", CAPACITY_DATA_PATH);
            perror("Error opening file "CAPACITY_DATA_PATH);  
            return FAILED;  
        }

        if (fscanf(file, "%lf", &capacity) != 1) {
            printf("Error reading capacity from file %s\n", CAPACITY_DATA_PATH);
            perror("Error reading capacity from file "CAPACITY_DATA_PATH);  
            fclose(file);  
            return FAILED;   
        }

        fclose(file);  

        return capacity;  
    }

    void modify_ini_file(const char* filename, configuration* config) {
        FILE* file = fopen(filename, "w");
        if (!file) {
            printf("Unable to open file  \"%s\" for writing\n", filename);
            perror("Unable to open file for writing");  
            return;
        }

        fprintf(file, "[simulation]\n");
        fprintf(file, "tenant_number = %d\n", config->tenant_number);  
        fprintf(file, "simulation_time = %ld\n", config->simulation_time);  
        fprintf(file, "error = %f\n", config->error);  
        fprintf(file, "data_path = %s\n", config->data_path);  
        fprintf(file, "unit = %d\n", config->unit);

        // Write the [traffic] section
        fprintf(file, "[traffic]\n");
        fprintf(file, "input_rate = %d\n", config->input_rate);
        fprintf(file, "traffic_mode = %d\n", config->traffic_mode);  
        fprintf(file, "mean = %d\n", config->mean); 
        fprintf(file, "standard_deviation = %d\n", config->standard_deviation);  
        fprintf(file, "packet_size = %d\n", config->packet_size);  

        fprintf(file, "naughty_mean = %d\n", config->naughty_mean);  
        fprintf(file, "naughty_standard_deviation = %d\n", config->naughty_standard_deviation);  
        fprintf(file, "naughty_tenant_number = %d\n", config->naughty_tenant_number);  

        // Write the [GCRA] section
        fprintf(file, "[GCRA]\n");
        fprintf(file, "tau_1 = %ld\n", config->tau_1);  
        fprintf(file, "tau_2 = %ld\n", config->tau_2);  

        // Close the file
        fclose(file); 
    }

    void show_configuration(const configuration config){
        printf("- Simulation :\n");
        printf("| tenant number              : %-d\n", config.tenant_number);
        printf("| simulation time            : %-ld\n", config.simulation_time);
        printf("| error                      : %-f\n", config.error);
        printf("| data path                  : %-s\n", config.data_path);
        
        switch(config.unit){
            case UNIT_MBPS:
                printf("| unit                       : Mbps\n");
                break;
            case UNIT_KBPS:
                printf("| unit                       : kbps\n");
                break;
            default:
                printf("| unit                       : Mbps\n");

        }

        printf("- Traffic :\n");
        printf("| input rate                 : %-ld\n", config.input_rate);
        printf("| traffic mode               : %-d\n", config.traffic_mode);
        printf("| mean                       : %-d\n", config.mean);
        printf("| standard deviation         : %-d\n", config.standard_deviation);
        printf("| packet size                : %-d\n", config.packet_size);
        
        if(config.traffic_mode>TRAFFIC_MODE_GAUSSIAN){
            printf("| naughty mean               : %-d\n", config.naughty_mean);
            printf("| naughty standard deviation : %-d\n", config.naughty_standard_deviation);
            printf("| naughty tenant number      : %-d\n", config.naughty_tenant_number);
        }

        printf("- GCRA :\n");
        printf("| tau_1                      : %-ld\n", config.tau_1);
        printf("| tau_2                      : %-ld\n", config.tau_2);
    }

    long obtainUnit(const configuration config){
        switch (config.unit){
            case UNIT_MBPS:
                // printf("unit = %ld\n", MBPS);
                return MBPS;
                break;
            case UNIT_KBPS:
                // printf("unit = %ld\n", KBPS);
                return KBPS;
                break;
            default:
                // printf("unit = %ld\n", MBPS);
                return MBPS;
        }
    }
#endif

// Test Module
// #define REDUCTION

// #include "../Enthalten/include.h"        
// #include "../Enthalten/config.h"
// #include "./inih/ini.h"

// #define CONFIGURATION_PATH "../configuration/simple_V10_1.ini"

// // gcc ./simple_V10_1.c inih/ini.c -o ../Ausführung/simple_V10_1
// // ../Ausführung/simple_V10_1 

// int main(int argc, char *argv[]){
//     configuration config;

//     if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
//         printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
//         return EXIT_FAILURE;
//     }

//     show_configuration(config);

//     config.packet_size = 512;
//     modify_ini_file(CONFIGURATION_PATH, &config);

//     reduction_inif_file(CONFIGURATION_PATH);
//     return EXIT_SUCCESS;
// }