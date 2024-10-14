
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

// gcc ./simple_V5_variation_observation.c inih/ini.c -o ../Ausführung/simple_V5_variation_observation -lcurl -lgsl -lgslcblas -lm
// ../Ausführung/simple_V5_variation_observation 

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

    // int parameter = _CODE_TENANT_NUMBER;    // Change
    // int parameter = _CODE_ERROR;    // Change
    // int parameter = _CODE_MEAN;    // Change
    // int parameter = _CODE_STANDARD_DEVIATION;    // Change
    int parameter = _CODE_BUCKET_DEPTH;    // Change
    // int parameter = _CODE_LEAKAGE_RATE;    // Change
    // int parameter = _CODE_TIME_INTERVAL;
    int length;  // Change

    system("python3 "PYTHON_PATH" 15");
    sprintf(command, "gcc ./simple_V5_variation.c inih/ini.c -o ../Ausführung/simple_V5_variation -lcurl -lgsl -lgslcblas -lm");
    system(command);

    double fraction = 2;
    switch (parameter){
        case _CODE_TENANT_NUMBER:
            break;
        case _CODE_ERROR:
            break;
        case _CODE_MEAN:
            break;
        case _CODE_STANDARD_DEVIATION:
            break;
        case _CODE_BUCKET_DEPTH:
            int time = atoi(argv[1]);
            
            for(int index = 0;index<time;index++){
                system("../Ausführung/simple_V5_variation");
                // printf("a=%d\n", index);
                system("python3 "PYTHON_PATH" 14");
            }
            break;
        case _CODE_LEAKAGE_RATE:
            break;
        case _CODE_TIME_INTERVAL:
            break;
        default:
            return EXIT_SUCCESS;
            break;
    }

    char title[MAX_PATH_LENGTH];
    switch (parameter){
        case _CODE_TENANT_NUMBER:
            sprintf(title, "Tenant_Number");
            break;
        case _CODE_ERROR:
            sprintf(title, "Error");
            break;
        case _CODE_MEAN:
            sprintf(title, "Mean");
            break;
        case _CODE_STANDARD_DEVIATION:
            sprintf(title, "Covariance");
            break;
        case _CODE_BUCKET_DEPTH:
            sprintf(title, "Bucket_Depth");
            break;
        case _CODE_LEAKAGE_RATE:
            sprintf(title, "Leakage_Rate");
            break;
        case _CODE_TIME_INTERVAL:
            sprintf(title, "Time_Interval");
            break;
        default:
            return EXIT_SUCCESS;
            break;                                                                                                                                                                                                                                    
    }

    
    system("python3 "PYTHON_PATH" 16");
    return EXIT_SUCCESS;
}