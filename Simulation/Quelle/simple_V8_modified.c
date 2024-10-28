
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V8.ini"
#define PYTHON_PATH "./simple_V8.py"
#define DATA_STORED_PATH "../Datei/simple_V8/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V8/traffic.csv"
// gcc ./simple_V8_modified.c inih/ini.c -o ../Ausführung/simple_V8_modified -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V8_modified c

#define _CODE_TENANT_NUMBER 0
#define _CODE_MEAN 1
#define _CODE_STANDARD_DEVIATION 2
#define _CODE_TAU 3
#define _CODE_L 4

#define REDUCTION

typedef struct{
    long tau;    
    long l;   
    long last_time;
    int record;
    long virtual_time;
    long allow_time;
}GCRA;

GCRA initGCRA(long tau, long l){
    GCRA gcra;
    gcra.tau = tau;
    gcra.l = l;
    gcra.last_time = 0;
    gcra.record = 0;
    gcra.allow_time = 0;
    gcra.virtual_time = 0;
    return gcra;
}


int main(int argc, char *argv[]){
    int tenant_number;  /** @brief The number of tenants in the system. **/
    long time_interval; /** @brief The time interval for token bucket processing (in milliseconds). **/
    double error;
    double capacity;
    int gaussian = 0;
    double mean;    /** @brief The mean value for traffic generation. **/
    double standard_deviation;  /** @brief The standard deviation used for traffic generation. **/
    double naughty_mean;
    double naughty_standard_deviation;
    int naughty_tenant_number;

    double bucket_depth;    /** @brief The maximum depth of the token bucket. **/
    double leakage_rate;    /** @brief The rate at which tokens leak from the bucket (tokens per time interval). **/
    long tau;
    long l;

    configuration config;
    char command[MAX_COMMAND_LENGTH];

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
        printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }

    switch(atoi(argv[1])){
        case _CODE_TENANT_NUMBER:
            config.tenant_number = atoi(argv[2]);
        break;
        case _CODE_MEAN:
            config.mean = atof(argv[2]);
        break;
        case _CODE_STANDARD_DEVIATION:
            config.standard_deviation = atof(argv[2]);
        break;
        case _CODE_TAU:
            config.tau = strtol(argv[2], NULL, 10);
        break;
        case _CODE_L:
            config.l = strtol(argv[2], NULL, 10);
        break;
        default:
            config.capacity = 360;
            config.tenant_number = 3;
            config.error = 0.001;
            config.mean = 120;
            config.standard_deviation = 40;
            config.bucket_depth = 200;
            config.leakage_rate = 120;
            config.tau = 8333333;
            config.l = 16666666;
        break;
    }

    tenant_number = config.tenant_number;      /** @brief 2 : Number of tenants **/
    time_interval = config.time_interval;     /** @brief 3 : Simulation time (with time units) **/
    gaussian = config.gaussian;
    mean = config.mean;               /** @brief 5 : Mean of the traffic **/
    standard_deviation = config.standard_deviation; /** @brief 6 : Standard derivation(Covariance) of the traffic **/
    bucket_depth = config.bucket_depth;       /** @brief 7 : GCRA bucket size(depth) **/
    leakage_rate = config.leakage_rate;       /** @brief 8 : GCRA leakage rate **/
    error = config.error;  
    naughty_mean = config.naughty_mean;
    naughty_standard_deviation = config.naughty_standard_deviation;
    naughty_tenant_number = config.naughty_tenant_number;
    tau = config.tau;
    l = config.l;

    system("python3 " PYTHON_PATH " 0 "CONFIGURATION_PATH);
    capacity = read_double_from_file("../Datei/objective.txt");
    printf("Capacity = "INFORM_DOUBLE_FORMAT"\n", capacity);

    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index = 0; index < tenant_number; index++){
        *(tenants + index) = (struct Tenant){index, NULL};
        double* traffic;

        if(gaussian == 0)
            traffic = generateRegularTraffic_(time_interval, mean, standard_deviation, index);
        else if(gaussian == 1)
            traffic = generateNormalDistribution(time_interval, mean, standard_deviation);
        else if(gaussian == 2){
            if(index<naughty_tenant_number){
                traffic = generateNaughtyTraffic(time_interval, naughty_mean, naughty_standard_deviation);
                // printf("index = %d | %d\n", index, naughty_tenant_number);
            }
            else {
                traffic = generateRegularTraffic_(time_interval, mean, standard_deviation, index);
                // printf("index = %d / %d\n", index, naughty_tenant_number);
            }
        }
        
        // printf("%d\n", config.time_interval);
        (*(tenants + index)).traffic = traffic;

        translateTimestamps(traffic, time_interval, &(*(tenants + index)));
    }

    GCRA* gcras = (GCRA*)malloc(sizeof(GCRA)*tenant_number);

    for(int index = 0; index < tenant_number; index++){
        *(gcras+index) = initGCRA(tau, l);
        for(int time_stamp = 0;time_stamp<tenants[index].timestamps_length;time_stamp++){

            // if(gcras[index].last_time == 0){
            //     gcras[index].last_time = tenants[index].timestamps[time_stamp];
            //     gcras[index].tau = tenants[index].timestamps[time_stamp] + gcras[index].tau;
            // }
            int accept = 0;

            // printf("v= %lld a = %lld t = %lld %d\n", gcras[index].virtual_time, gcras[index].allow_time, tenants[index].timestamps[time_stamp],
            //     (tenants[index].timestamps[time_stamp] >= gcras[index].allow_time));

            if(tenants[index].timestamps[time_stamp] >= gcras[index].allow_time){
                accept = ACCEPTED_CODE;

                long _virtual_time = gcras[index].virtual_time;

                if(_virtual_time>tenants[index].timestamps[time_stamp])
                    gcras[index].virtual_time = _virtual_time+gcras[index].tau;
                else gcras[index].virtual_time = tenants[index].timestamps[time_stamp]+gcras[index].tau;

                gcras[index].allow_time = (long)(_virtual_time - (long)(gcras[index].l));
            }else{
                accept = DROPPED_CODE;
            }

            tenants[index].accpectance[time_stamp] = accept;
        }
        char path[MAX_PATH_LENGTH];
        sprintf(path, DATA_STORED_PATH, index);
        recordTimestamps(*(tenants + index), path);

        // printf("%lld\n", gcras[index].tau);
        // printf("%lld\n", gcras[index].virtual_time);
    }

    double min_rate = 100.0;
    for(int index = 0; index < tenant_number; index++){
        int count = 0;
        for(int time_stamp = 0;time_stamp<tenants[index].timestamps_length;time_stamp++){
            count += tenants[index].accpectance[time_stamp];
            
        }
        // printf("%d\n", count);
        // printf("%d\n", tenants[index].timestamps_length);
        double rate = ((double)count/(double)tenants[index].timestamps_length);
        // printf("%f\n", rate);
        if(rate*100.0 < min_rate)
            min_rate = rate*100;
        printf("Tanant %2d : %f %\n", index, rate*100);

        // sprintf(command, "python3 " PYTHON_PATH " 1 %d", index);
        // system(command);
    }

    FILE* data_stored = fopen(BUCKET_DATA_STORED_PATH, "a+");
    fprintf(data_stored, "%s, %f, %f, %f\n", argv[2], capacity, (double)(capacity/((double)tenant_number)), min_rate);
    printf("%s, %f, %f, %f\n", argv[2], capacity, (double)(capacity/((double)tenant_number)), min_rate);
    fclose(data_stored);

#ifdef REDUCTION
            config.capacity = 360;
            config.tenant_number = 14;
            config.time_interval = 100;
            config.error = 0.001;
            config.mean = 120;
            config.standard_deviation = 40;
            config.bucket_depth = 285.5160353;
            config.leakage_rate = 120;
            config.naughty_mean = 150;
            config.naughty_standard_deviation = 10;
            config.naughty_tenant_number = 7;
            config.tau = 8333333;
            config.l = 16666666;
    modify_ini_file(CONFIGURATION_PATH, &config);
#endif


    return EXIT_SUCCESS;
}