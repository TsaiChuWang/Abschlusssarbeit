
#include "../Enthalten/include.h"        /**< General includes */
#include "../Enthalten/configuration.h" /**< Configuration header file */
#include "../Enthalten/tenant.h"         /**< Tenant data management */
#include "../Enthalten/token_bucket.h"   /**< Token bucket algorithm implementation */
#include "./inih/ini.h"

#define IMPORTANT_DATA_STORED_PATH "../Datei/simple_V5/important_data.csv"
#define CONFIGURATION_PATH "../configuration/simple_V5.ini"
#define PYTHON_PATH "./simple_V5.py"  /** @brief Path to the Python script for generating images or other visualization outputs. **/
// #define PRINT
#define END_PRINT

#define _CODE_CAPACITY 0
#define _CODE_TENANT_NUMBER 1
#define _CODE_ERROR 2
#define _CODE_MEAN 3
#define _CODE_STANDARD_DEVIATION 4
#define _CODE_BUCKET_DEPTH 5
#define _CODE_LEAKAGE_RATE 6
#define _CODE_TIME_INTERVAL 7

// gcc ./simple_V5_1_variation.c inih/ini.c -o ../Ausführung/simple_V5_1_variation -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V5_1_variation 

int main(int argc, char *argv[]){
    configuration config;
    char command[MAX_COMMAND_LENGTH];
    
    FILE* data_file;
    data_file = fopen(IMPORTANT_DATA_STORED_PATH, "w+");
    fprintf(data_file,"");
    fclose(data_file);

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0) {
      printf("Can't load configuration\"%s\"\n", CONFIGURATION_PATH);
      return EXIT_FAILURE;
    }

    sprintf(command, "gcc ./simple_V5_1_modeified.c inih/ini.c -o ../Ausführung/simple_V5_1_modeified -lcurl -lgsl -lgslcblas -lm");
    system(command);

    for(int tenant = 1; tenant<=config.tenant_number;tenant++){
        sprintf(command, "../Ausführung/simple_V5_1_modeified %d", tenant);
        system(command);
    }

    system("python3 " PYTHON_PATH " 18 ");
    system("python3 " PYTHON_PATH " 19 ");
    system("python3 " PYTHON_PATH " 20 ");
    return EXIT_SUCCESS;
}