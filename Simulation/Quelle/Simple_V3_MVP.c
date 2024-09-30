#include "../Enthalten/configuration.h"
#include "../Enthalten/include.h"
#include "../Enthalten/tenant.h"

#define DATA_STORED_PATH "../Datei/Simple_V3_MVP/traffic_%d.csv"

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
#endif
    }
        
    return EXIT_SUCCESS;
}