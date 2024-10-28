
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V8.ini"
#define PYTHON_PATH "./simple_V8.py"
#define DATA_STORED_PATH "../Datei/simple_V8/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V8/traffic.csv"
// gcc ./simple_V8_variation.c inih/ini.c -o ../Ausführung/simple_V8_variation -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V8_variation 0

#define _CODE_TENANT_NUMBER 0
#define _CODE_MEAN 1
#define _CODE_STANDARD_DEVIATION 2
#define _CODE_TAU 3
#define _CODE_L 4


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
    FILE* data_file;
    data_file = fopen(BUCKET_DATA_STORED_PATH, "w+");
    fprintf(data_file,"");
    fclose(data_file);

    system("gcc ./simple_V8_modified.c inih/ini.c -o ../Ausführung/simple_V8_modified -lcurl -lgsl -lgslcblas -lm");
    int parameter = atoi(argv[1]);
    char command[MAX_COMMAND_LENGTH];

    switch (parameter)
    {
        case _CODE_TENANT_NUMBER:
            for(int tenant_number = 1;tenant_number<50;tenant_number++){
                sprintf(command, "../Ausführung/simple_V8_modified %d %d", parameter, tenant_number);
                system(command);
            }

            sprintf(command, "python3 " PYTHON_PATH " 2 Tenant_Number");
            system(command);
            break;
        
        default:
            return EXIT_FAILURE;
            break;
    }

    return EXIT_SUCCESS;
}