#include "../Enthalten/configuration.h"
#include "../Enthalten/include.h"
#include "../Enthalten/tenant.h"
#include "../Enthalten/token_bucket.h"

#define DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic.csv"
#define PYTHON_CVXPY_PATH "./CVXPY.py"
#define PYTHON_IMAGE_PATH "./Simgle_V3_MVP.py"
// gcc ./Simple_V3_MVP.c -o ../Ausführung/Simple_V3_MVP -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/Simple_V3_MVP
// #define INFORM_BUCKET_SITUATION

double read_double_from_file(const char *filename) {
    FILE *file;
    double value;

    // Open the file in read mode
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1.0;  // Return an error value if file can't be opened
    }

    // Read the double value from the file
    if (fscanf(file, "%lf", &value) != 1) {
        perror("Error reading double from file");
        fclose(file);
        return -1.0;   // Return an error value if reading fails
    }

    // Close the file
    fclose(file);

    return value;
}

int main(int argc, char *argv[]){

    double capacity = 360;
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
#ifdef INFORM_BUCKET_SITUATION
            printf(INFORM_TRAFFIC_FORMAT" ", (*(tenants+index)).traffic[time_stamp]);
            printf(INFORM_TRAFFIC_FORMAT" ", leakage_traffic);
            printf(INFORM_TRAFFIC_FORMAT" ",  (*(buckets+index)).bucket_capacity);
#endif
        }
#ifdef INFORM_BUCKET_SITUATION
        printf("\n");
#endif
#ifdef RECORD
        fprintf(traffic_file, "\n");
#endif
    }
#ifdef RECORD
    fclose(traffic_file);

    for(int index=0;index<tenant_number;index++){
        char command[MAX_COMMAND_LENGTH];
        sprintf(command, "python3 "PYTHON_IMAGE_PATH" 2 %d", index);
        system(command);
    }

    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "python3 "PYTHON_IMAGE_PATH" 3 %f", capacity);
    system(command);
#endif
#ifndef RECORD
    char command[MAX_COMMAND_LENGTH];
#endif
    sprintf(command, "python3 ./CVXPY.py %f", capacity);
    system(command);
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));
    
    capacity = read_double_from_file("../Datei/objective.txt")*capacity;
    sprintf(command, "python3 "PYTHON_IMAGE_PATH" 3 %f", capacity);
    system(command);

    sprintf(command, "python3 ./CVXPY.py %f", capacity);
    system(command);
    printf("alpha = %f\n", read_double_from_file("../Datei/objective.txt"));

    return EXIT_SUCCESS;
}