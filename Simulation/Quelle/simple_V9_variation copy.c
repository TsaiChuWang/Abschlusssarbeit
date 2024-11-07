
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define CONFIGURATION_PATH "../configuration/simple_V9.ini"
#define PYTHON_PATH "./simple_V9.py"
#define DATA_STORED_PATH "../Datei/simple_V9/traffic_%d.csv"
#define BUCKET_DATA_STORED_PATH "../Datei/simple_V9/traffic.csv"
// gcc ./simple_V9_variation.c inih/ini.c -o ../Ausführung/simple_V9_variation -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V9_variation 0

#define _CODE_TENANT_NUMBER 0
#define _CODE_MEAN 1
#define _CODE_STANDARD_DEVIATION 2
#define _CODE_TAU 3
#define _CODE_L 4
#define _CODE_NAUGHTY_NUMBER 5

int main(int argc, char *argv[]){
    FILE* data_file;
    data_file = fopen(BUCKET_DATA_STORED_PATH, "w+");
    fprintf(data_file,"");
    fclose(data_file);

    system("gcc ./simple_V9_modified.c inih/ini.c -o ../Ausführung/simple_V9_modified -lcurl -lgsl -lgslcblas -lm");
    int parameter = atoi(argv[1]);
    char command[MAX_COMMAND_LENGTH];

    switch (parameter)
    {
        case _CODE_TENANT_NUMBER:
            for(int tenant_number = 1;tenant_number<50;tenant_number++){
                sprintf(command, "../Ausführung/simple_V9_modified %d %d", parameter, tenant_number);
                system(command);
            }

            sprintf(command, "python3 " PYTHON_PATH " 2 Tenant_Number");
            system(command);
            break;

        case _CODE_TAU:
            long last = 3083600;
            for(long tau = 3083400;tau<=last;tau+=10){
                sprintf(command, "../Ausführung/simple_V9_modified %d %d", parameter, tau);
                system(command);
            }

            sprintf(command, "python3 " PYTHON_PATH " 2 Tau");
            system(command);
            break;

        case _CODE_L:
            for(long l = 50000;l<25000000;l+=10000){
                sprintf(command, "../Ausführung/simple_V9_modified %d %d", parameter, l);
                system(command);
            }

            sprintf(command, "python3 " PYTHON_PATH " 2 L");
            system(command);
            break;
        case _CODE_NAUGHTY_NUMBER:
            for(int tenant=1;tenant<=100;tenant++){
                sprintf(command, "../Ausführung/simple_V9_modified %d %d", parameter, tenant);
                system(command);
            }

            sprintf(command, "python3 " PYTHON_PATH " 2 Naughty_Tenant_Number");
            system(command);
            break;

        default:
            return EXIT_FAILURE;
            break;
    }

    return EXIT_SUCCESS;
}