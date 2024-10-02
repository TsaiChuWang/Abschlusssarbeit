#include "../Enthalten/configuration.h"
#include "../Enthalten/include.h"
#include "../Enthalten/tenant.h"
#include "../Enthalten/token_bucket.h"

#define DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic.csv"

#define PYTHON_IMAGE_PATH "./Simgle_V3_MVP.py"
// gcc ./Simple_V3_MVP.c -o ../Ausführung/Simple_V3_MVP -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/Simple_V3_MVP

int main(int argc, char *argv[]){

    int tenant_number = 3;
    long time_interval = 100;
    double mean = 120;
    double standard_deviation = 40;

    // Topologie erstellen
    struct Tenant* tenants = (struct Tenant*)malloc(sizeof(struct Tenant)*tenant_number);
    for(int index=0;index<tenant_number;index++){
        *(tenants+index) = (struct Tenant){index, NULL};
        double* traffic = generateRegularTraffic(time_interval, mean, standard_deviation);
        (*(tenants+index)).traffic = traffic;

#ifdef RECORD
        FILE* traffic_file;
        char path[MAX_PATH_LENGTH];
        sprintf(path, DATA_STORED_PATH, index);
        traffic_file = fopen(path, "w+");
        
        for(int time_stamp = 0;time_stamp<time_interval;time_stamp++)
            fprintf(traffic_file, INFORM_DOUBLE_FORMAT"\n", (*(tenants+index)).traffic[time_stamp]);
        
        fclose(traffic_file);

        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "python3 "PYTHON_IMAGE_PATH" 1 %d", index);
        system(command);
#endif
    }

#ifdef RECORD
        FILE* traffic_file;
        traffic_file = fopen(BUCKET_DATA_STORED_PATH, "w+");
#endif

    struct Token_Bucket* buckets =  createMultipleTokenBucket_SameParamter(200, 120, tenant_number);
    for(int time_stamp = 0;time_stamp<time_interval;time_stamp++){
        for(int index=0;index<tenant_number;index++){
            double traffic = (*(tenants+index)).traffic[time_stamp];
            double leakage_traffic = 0.0;

            if(traffic > mean+standard_deviation){
                (*(buckets+index)).bucket_capacity += (*(buckets+index)).leakage_rate;
                if((*(buckets+index)).bucket_capacity > (*(buckets+index)).bucket_depth)
                    (*(buckets+index)).bucket_capacity = (*(buckets+index)).bucket_depth;
                goto end_distinguish;
            }

            if((*(buckets+index)).bucket_capacity + (*(buckets+index)).leakage_rate - traffic < 0){
                leakage_traffic = (*(buckets+index)).bucket_capacity + (*(buckets+index)).leakage_rate;
                (*(buckets+index)).bucket_capacity = 0;
                goto end_distinguish;
            }

            if((*(buckets+index)).bucket_capacity + (*(buckets+index)).leakage_rate - traffic > 0){
                leakage_traffic = traffic;
                (*(buckets+index)).bucket_capacity = (*(buckets+index)).bucket_capacity + (*(buckets+index)).leakage_rate - traffic;
                if((*(buckets+index)).bucket_capacity > (*(buckets+index)).bucket_depth)
                    (*(buckets+index)).bucket_capacity = (*(buckets+index)).bucket_depth;
                goto end_distinguish;
            }

            if(traffic <= mean-standard_deviation){
                leakage_traffic = traffic;
                (*(buckets+index)).bucket_capacity += (*(buckets+index)).bucket_capacity + (*(buckets+index)).leakage_rate - traffic;
                if((*(buckets+index)).bucket_capacity > (*(buckets+index)).bucket_depth)
                    (*(buckets+index)).bucket_capacity = (*(buckets+index)).bucket_depth;
                goto end_distinguish;
            }

end_distinguish : 
#ifdef RECORD
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT", ", (*(tenants+index)).traffic[time_stamp]);
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT", ", leakage_traffic);
            fprintf(traffic_file, INFORM_TRAFFIC_FORMAT", ",  (*(buckets+index)).bucket_capacity);
#endif
            printf(INFORM_TRAFFIC_FORMAT" ", (*(tenants+index)).traffic[time_stamp]);
            printf(INFORM_TRAFFIC_FORMAT" ", leakage_traffic);
            printf(INFORM_TRAFFIC_FORMAT" ",  (*(buckets+index)).bucket_capacity);
        }
        printf("\n");
#ifdef RECORD
        fprintf(traffic_file, "\n");
#endif
    }
#ifdef RECORD
    fclose(traffic_file);
#endif
    return EXIT_SUCCESS;
}