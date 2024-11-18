
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V9.ini"
#define PYTHON_PATH "./simple_V9.py"
#define DATA_STORED_PATH "../Datei/simple_V9/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V9/traffic.csv"
// gcc ./simple_V9.c inih/ini.c -o ../Ausführung/simple_V9 -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V9 c
// #define gcras[index].tau 8333333
// #define L 16666666
#define PACKET_SIZE 256

typedef struct{
    long tau;    
    long l;   
    long last_time;
    int record;
    long virtual_time;
    long allow_time;

    long x;
    long _x;
}GCRA;

GCRA initGCRA(long tau, long l){
    GCRA gcra;
    gcra.tau = tau;
    gcra.l = l;
    
    gcra.record = 0;
    gcra.allow_time = 0;
    gcra.virtual_time = 0;

    gcra.last_time = 0;
    gcra.x = 0;
    gcra._x = 0;
    return gcra;
}

long max(long a, long b){
    if(a>b)
        return a;
    return b;
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

    if(argc<2){ // Argument parsing and default assignments
        tenant_number = 3;
        time_interval = 100;
        mean = 120;
        standard_deviation = 40;
        bucket_depth = 40;
        leakage_rate = 120;
        error = 0.01;
        naughty_mean = 150;
    }
    
    if(strcmp(argv[1], "c")==0){
        if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
            printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
            return EXIT_FAILURE;
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
    }
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
            int accept = 0;

            gcras[index]._x = gcras[index].x - (tenants[index].timestamps[time_stamp]- gcras[index].last_time)
                *(((int)mean*PACKET_SIZE)/ONE_SECOND_IN_NS);
            
            if(gcras[index]._x > gcras[index].tau){
                accept = DROPPED_CODE;
            }else{
                accept = ACCEPTED_CODE;
                gcras[index].x = max((long)0, gcras[index]._x)+PACKET_SIZE;
                gcras[index].last_time = tenants[index].timestamps[time_stamp];
            }

            
            tenants[index].accpectance[time_stamp] = accept;
        }

        char path[MAX_PATH_LENGTH];
        sprintf(path, DATA_STORED_PATH, index);
        recordTimestamps(*(tenants + index), path);

        // printf("%lld\n", gcras[index].tau);
        // printf("%lld\n", gcras[index].virtual_time);
    }

    for(int index = 0; index < tenant_number; index++){
        int count = 0;
        for(int time_stamp = 0;time_stamp<tenants[index].timestamps_length;time_stamp++){
            count += tenants[index].accpectance[time_stamp];
            
        }
        // printf("%d\n", count);
        // printf("%d\n", tenants[index].timestamps_length);
        double rate = ((double)count/(double)tenants[index].timestamps_length);
        // printf("%f\n", rate);

        printf("Tanant %2d : %f %\n", index, rate*100);

        sprintf(command, "python3 " PYTHON_PATH " 1 %d", index);
        system(command);
    }

    

    return EXIT_SUCCESS;
}