#define UNIFORM_ALL_COMPLIANT 0 ///< Define a constant for the uniform all-compliant case.

#define UNIFORM_DISTRIBUTION_NAUGHTY 1

#define UNIFORM_DISTRIBUTION_NAUGHTY_DIFFERENT_MODE 2
#define UNIFORM_DISTRIBUTION_NAUGHTY_DIFFERENT_NAUGHTY_NUMBER 3

#define BURSTY_ALL_DIFFERENT_R 4
#define BURSTY_REGULAR 7
#define BURSTY_NAUGHTY 3
#define BRUSTY_DIFFERENT_R 8

#define TRAFFIC_BRUSTY_SITUATION 4
#define BURSTY_ALL_DIFFERENT_R_NAUGHTY 5

#define INTERVAL 0
#define NAUGHTY 1

#define REDUCTION

#include "../include/general.h"       ///< Include general definitions and declarations.
#include "./inih/ini.h"               ///< Include INI file handling library.
#include "../include/configuration.h" ///< Include configuration management definitions.
#include "../include/packets_count.h" ///< Include packet counting utilities.

#define CONFIGURATION_PATH "../configuration/main.ini" ///< Define the path to the main configuration file.

// Compilation command:
// gcc ./skript.c inih/ini.c -o ../execution/skript -lm
// Execution command:
// ../execution/skript

int main(int argc, char *argv[])
{
    /**
     * @brief Initializes variables for processing data and configuration.
     *
     * This section of code declares and initializes various variables used for executing the main program,
     * including command buffers, step sizes, and file paths.
     */

    char command[MAX_COMMAND_LENGTH];                           ///< Buffer for storing system command to compile the program.
    system("gcc ./main.c inih/ini.c -o ../execution/main -lm"); ///< Compile the main program with required source files.

    long step = 256;                     ///< Step size for processing (e.g., data chunks).
    double state_r_step = 0.1;           ///< Step increment for state-related calculations.
    int step_naughty_tenant_number = 25; ///< Number of non-compliant (naughty) tenants.

    char name[MAX_NAME_LENGTH];               ///< Buffer for storing the name of the configuration.
    char configuration_path[MAX_PATH_LENGTH]; ///< Buffer for the path to the configuration file.
    char data_path[MAX_PATH_LENGTH];          ///< Buffer for the path to the data directory.

    char file_path[MAX_PATH_LENGTH]; ///< Buffer to store the path for the output file.
    FILE *file;                      ///< File pointer for the output file.

    /**
     * @brief Executes the main processing loop for the uniform all-compliant configuration.
     *
     * This section of code processes command-line arguments to determine the configuration type,
     * loads the appropriate settings, prepares the data directory, and executes the main program
     * along with Python scripts for data analysis.
     *
     * @param argc The number of command-line arguments.
     * @param argv The array of command-line argument strings.
     * @return Returns EXIT_FAILURE if an error occurs; otherwise, it continues execution.
     */

    switch (atoi(argv[1])) ///< Convert the first command-line argument to an integer.
    {
    case UNIFORM_ALL_COMPLIANT:
        strcpy(name, "uniform_all_compliant");                        ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        configuration config; ///< Structure to hold the configuration settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);     ///< Construct the path for the data directory.
        config.data_path = data_path;               ///< Set the data path in the configuration structure.
        config.traffic_mode = TRAFFIC_MODE_UNIFORM; ///< Set the traffic mode to uniform.
        modify_ini_file(configuration_path, &config);

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for statistics.

        // Loop through tau values and execute the main program for each.
        for (long tau = 0; tau <= 25600; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_CHART_PATH, name, configuration_path); ///< Command to run the Python script for data analysis.
            system(command);                                                                                ///< Execute the command.
        }
        break;

    default:
        printf(RED_ELOG "CODE DOES NOT EXIST!\n" RESET); ///< Error message for unsupported configuration.
        return EXIT_FAILURE;                             ///< Exit with failure status for unknown configurations.
    }

    // // configuration.h
    // configuration config;

    // if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    // {
    //     printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
    //     return EXIT_FAILURE;
    // }

    // char file_average_loss_path[MAX_PATH_LENGTH];
    // sprintf(file_average_loss_path, "%s/record_average_loss.csv", config.data_path);
    // // printf("%s(../data/main/record_average_loss.csv)\n", file_average_loss_path);

    // FILE *file = fopen(file_average_loss_path, "w+");
    // if (!file)
    // {
    //     perror("Error opening file"); /**< Handle file open errors. */
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(file, RECORD_AVERAGE_LOSS_HEADER);
    // fclose(file);

    // char file_regular_and_naughty_tau_path[MAX_PATH_LENGTH];
    // sprintf(file_regular_and_naughty_tau_path, "%s/regular_and_naughty_tau.csv", config.data_path);
    // // printf("%s(../data/main/regular_and_naughty_tau.csv)\n", file_regular_and_naughty_tau_path);

    // file = fopen(file_regular_and_naughty_tau_path, "w+");
    // if (!file)
    // {
    //     perror("Error opening file"); /**< Handle file open errors. */
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(file, RECORD_REGULAR_AND_NAUGHTY_TAU_HEADER);
    // fclose(file);

    // char file_regular_and_naughty_all_path[MAX_PATH_LENGTH];
    // sprintf(file_regular_and_naughty_all_path, "%s/regular_and_naughty_all.csv", config.data_path);
    // // printf("%s(../data/main/regular_and_naughty_all.csv)\n", file_regular_and_naughty_all_path);

    // file = fopen(file_regular_and_naughty_all_path, "w+");
    // if (!file)
    // {
    //     perror("Error opening file"); /**< Handle file open errors. */
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(file, RECORD_REGULAR_AND_NAUGHTY_ALL_HEADER);
    // fclose(file);

    // char file_packet_situation_path[MAX_PATH_LENGTH];
    // sprintf(file_packet_situation_path, "%s/record_packet_situation.csv", config.data_path);
    // // printf("%s(../data/main/record_packet_situation.csv)\n", file_packet_situation_path);

    // file = fopen(file_packet_situation_path, "w+");
    // if (!file)
    // {
    //     perror("Error opening file"); /**< Handle file open errors. */
    //     exit(EXIT_FAILURE);
    // }
    // fprintf(file, RECORD_REGULAR_AND_NAUGHTY_TAU_HEADER);
    // fclose(file);

    // reduction_inif_file(CONFIGURATION_PATH);

    // switch (atoi(argv[1]))
    // {

    // case UNIFORM_DISTRIBUTION_NAUGHTY:
    //     config.traffic_mode = TRAFFIC_MODE_NAUGHTY;
    //     config.naughty_mean = 155;
    //     config.naughty_mode = 2;
    //     for (long tau = 0; tau <= 25600; tau += step)
    //     {
    //         config.tau = tau;
    //         modify_ini_file(CONFIGURATION_PATH, &config);

    //         system("../execution/main");

    //         system("python3 ../python/average_loss.py 1");
    //         system("python3 ../python/regular_and_naughty_tau.py 0");
    //     }

    //     system("rm -r ../data/unifrom_naughty");
    //     sprintf(command, "cp -R %s ../data/unifrom_naughty", config.data_path);
    //     system(command);

    //     break;
    // case UNIFORM_DISTRIBUTION_NAUGHTY_DIFFERENT_MODE:
    //     config.traffic_mode = TRAFFIC_MODE_NAUGHTY;
    //     config.naughty_mean = 155;

    //     system("rm -r ../data/unifrom_naughty_naughty_mode");
    //     for (int i = 0; i < 3; i++)
    //     {
    //         config.naughty_mode = i;
    //         sprintf(command, "mkdir ../data/unifrom_naughty_naughty_mode/%d", i);
    //         system(command);

    //         for (long tau = 0; tau <= 25600; tau += step)
    //         {
    //             config.tau = tau;
    //             modify_ini_file(CONFIGURATION_PATH, &config);

    //             system("../execution/main");

    //             system("python3 ../python/average_loss.py 1");

    //             sprintf(command, "python3 ../python/regular_and_naughty_tau.py 2 naughty_mode=%d", i);
    //             system(command);
    //         }

    //         sprintf(command, "cp -R %s ../data/unifrom_naughty_naughty_mode/%d", config.data_path, i);
    //         system(command);
    //     }

    //     break;
    // case UNIFORM_DISTRIBUTION_NAUGHTY_DIFFERENT_NAUGHTY_NUMBER:
    //     config.traffic_mode = TRAFFIC_MODE_NAUGHTY;
    //     config.naughty_mean = 155;

    //     system("rm -r ../data/unifrom_naughty_different_average");
    //     system("mkdir ../data/unifrom_naughty_different_average");

    //     for (long tau = 0; tau <= 25600; tau += step)
    //     {
    //         config.naughty_mode = NAUGHTY_MODE_BEFORE;
    //         config.naughty_tenant_number = 1;
    //         config.tau = tau;
    //         modify_ini_file(CONFIGURATION_PATH, &config);

    //         system("../execution/main");

    //         system("python3 ../python/average_loss.py 1");

    //         sprintf(command, "python3 ../python/regular_and_naughty_tau.py 1");
    //         system(command);

    //         sprintf(command, "python3 ../python/regular_and_naughty_all.py 1");
    //         system(command);
    //     }

    //     for (int naughty_tenant_number = 25; naughty_tenant_number < config.tenant_number; naughty_tenant_number += step_naughty_tenant_number)
    //     {
    //         for (long tau = 0; tau <= 25600; tau += step)
    //         {
    //             config.naughty_mode = NAUGHTY_MODE_AVERAGE;
    //             config.tau = tau;
    //             config.naughty_tenant_number = naughty_tenant_number;
    //             modify_ini_file(CONFIGURATION_PATH, &config);

    //             system("../execution/main");

    //             system("python3 ../python/average_loss.py 1");

    //             sprintf(command, "python3 ../python/regular_and_naughty_tau.py 1");
    //             system(command);

    //             sprintf(command, "python3 ../python/regular_and_naughty_all.py 1");
    //             system(command);
    //         }
    //     }

    //     for (long tau = 0; tau <= 25600; tau += step)
    //     {
    //         config.naughty_mode = NAUGHTY_MODE_BEFORE;
    //         config.naughty_tenant_number = 99;
    //         config.tau = tau;
    //         modify_ini_file(CONFIGURATION_PATH, &config);

    //         system("../execution/main");

    //         system("python3 ../python/average_loss.py 1");

    //         sprintf(command, "python3 ../python/regular_and_naughty_tau.py 1");
    //         system(command);

    //         sprintf(command, "python3 ../python/regular_and_naughty_all.py 1");
    //         system(command);
    //     }

    //     sprintf(command, "cp -R %s ../data/unifrom_naughty_different_average", config.data_path);
    //     system(command);
    //     break;

    // case BURSTY_ALL_DIFFERENT_R:
    //     config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL;
    //     config.naughty_tenant_number = 0;
    //     config.naughty_mean = 155;
    //     for (long tau = 0; tau <= 25600; tau += step)
    //     {
    //         for (double state_r = 0.96875; state_r < 1; state_r += state_r_step)
    //         {
    //             config.tau = tau;
    //             config.state_r = state_r;
    //             modify_ini_file(CONFIGURATION_PATH, &config);

    //             system("../execution/main");

    //             system("python3 ../python/average_loss.py 2");
    //             system("python3 ../python/average_loss_all.py 2");
    //         }
    //     }

    //     break;
    // case BURSTY_ALL_DIFFERENT_R_NAUGHTY:
    //     config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL;
    //     config.input_rate = 335544320;
    //     config.naughty_mean = 155;
    //     config.naughty_tenant_number = 50;
    //     config.naughty_mode = 2;

    //     for (long tau = 0; tau <= 51200; tau += step)
    //     {
    //         for (double state_r = 0.484375; state_r < 1; state_r += state_r_step)
    //         {
    //             config.tau = tau;
    //             config.state_r = state_r;
    //             modify_ini_file(CONFIGURATION_PATH, &config);

    //             system("../execution/main");

    //             system("python3 ../python/average_loss.py 1");
    //             system("python3 ../python/regular_and_naughty_tau.py 2 burst");
    //             system("python3 ../python/regular_and_naughty_all.py 2");
    //         }
    //     }

    //     system("rm -r ../data/burst_different_r_naughty");
    //     sprintf(command, "cp -R %s ../data/burst_different_r_naughty", config.data_path);
    //     system(command);

    //     break;
    // case BRUSTY_DIFFERENT_R:
    //     config.traffic_mode = TRAFFIC_MODE_DIFFERENT_R;
    //     config.input_rate = 335544320;
    //     config.naughty_mean = 155;

    //     config.naughty_mode = 2;

    //     for (long tau = 0; tau <= 51200; tau += (step * 2))
    //     {
    //         for (int naughty_number = 25; naughty_number <= 75; naughty_number += 25)
    //         {
    //             config.naughty_tenant_number = naughty_number;
    //             for (double r = 0.6; r < 0.9; r += state_r_step)
    //                 for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step)
    //                 {
    //                     if (r == state_r)
    //                         continue;

    //                     config.tau = tau;
    //                     config.state_r = state_r;
    //                     modify_ini_file(CONFIGURATION_PATH, &config);

    //                     sprintf(command, "../execution/main %f", r);
    //                     system(command);

    //                     system("python3 ../python/average_loss.py 1");
    //                     system("python3 ../python/regular_and_naughty_tau.py 2 burst");
    //                     system("python3 ../python/regular_and_naughty_all.py 2");
    //                     // system("python3 ../python/different_r.py");
    //                 }
    //         }
    //     }

    //     system("rm -r ../data/different_r_dn");
    //     sprintf(command, "cp -R %s ../data/different_r_dn", config.data_path);
    //     system(command);
    //     break;
    // default:
    //     // config.traffic_mode = TRAFFIC_MODE_INTERVAL;
    //     // for (long tau = 0; tau <= 25600; tau += step)
    //     // {
    //     //     config.tau = tau;
    //     //     modify_ini_file(CONFIGURATION_PATH, &config);

    //     //     system("../execution/main");

    //     //     system("python3 ../python/average_loss.py 0");
    //     // }

    //     // system("rm -r ../data/unifrom_regular");
    //     // sprintf(command, "cp -R %s ../data/unifrom_regular", config.data_path);
    //     // system(command);
    //     // break;
    // }

    return 0;
}