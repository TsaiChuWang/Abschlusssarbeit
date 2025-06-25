#define UNIFORM_ALL_COMPLIANT 0                                           ///< Define a constant for the uniform all-compliant case.
#define UNIFORM_DISTRIBUTION_NONCOMPLIANT 1                               ///< Defines the constant for uniform distribution non-compliant configuration.
#define UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER 2 ///< Defines a constant for the number of non-compliant configurations in uniform distribution scenarios that are different from the standard.
#define BURST_ALL_COMPLIANT 3                                             ///< Defines a constant for the burst mode in compliant scenarios.
#define BURST_NONCOMPLIANT 4                                              ///< Constant definition for burst non-compliant state
#define BURST_NONCOMPLIANT_DIFFERENT_R 5                                  ///< Defines a constant for the burst mode in non-compliant scenarios with different R values.
#define BURST_NONCOMPLIANT_DIFFERENT_FIFO_QUEUE_BUFFER 6

#define UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER_FIFO 7
#define BURST_ALL_COMPLIANT_DIFFERENT_FIFO_QUEUE_BUFFER 8
#define BURST_NONCOMPLIANT_SIX_SEVEN 9
#define BURST_NONCOMPLIANT_7_25 10

#define SENSITIVE_NONCOMPLIANT_NUMBER 11
#define UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER_RATIO 12
#define UNIFORM_DISTRIBUTION_NONCOMPLIANT_RATIO 13
#define BURST_ALL_COMPLIANT_FIFO_RATIO 14
#define BURST_NONCOMPLIANT_RATIO 15
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

// #define NORMAL_MODE
// #define APPPEND_MODE
// #define VALIDATE_MODE

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

    long step = 256;                          ///< Step size for processing (e.g., data chunks).
    double state_r_step = 0.1;                ///< Step increment for state-related calculations.
    int step_noncompliant_tenant_number = 25; ///< Number of non-compliant (noncompliant) tenants.
    long default_tau = 5120;
    int step_upper_queu_buffer = 10;

    configuration config;                     ///< Structure to hold the configuration settings.
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

#ifndef APPEND_MODE
        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for statistics.
#endif

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
    case UNIFORM_DISTRIBUTION_NONCOMPLIANT:
        strcpy(name, "uniform_half_noncompliant");                    ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        config.traffic_mode = TRAFFIC_MODE_UNIFORM;   ///< Set the traffic mode to uniform.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

#ifndef APPEND_MODE
        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.
#endif
        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;    ///< Set the mode for non-compliant configuration.

        // Loop through tau values and execute the main program for each.
        for (long tau = 0; tau <= 51200; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_TAU_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }
        break; ///< End of the case for UNIFORM_DISTRIBUTION_NONCOMPLIANT.
    case UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER:
        strcpy(name, "uniform_different_noncompliant_number");        ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.
#ifdef NORMAL_MODE
        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.

        config.noncompliant_tenant_number = 1;                             ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT; ///< Set the mode for extreme non-compliance.

        // Loop through tau values and execute the main program for each.
        for (long tau = 0; tau <= 25600; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            // Run Python scripts for data analysis.
            sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_CHART_PATH, name, configuration_path); ///< Command for average loss chart.
            system(command);                                                                                ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_TAU_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }

        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE; ///< Set the mode for non-compliant configuration to average.

        // Loop through tenant numbers and tau values for further analysis.
        for (int noncompliant_number = 25; noncompliant_number <= 75; noncompliant_number += 25)
            for (long tau = 0; tau <= 25600; tau += step)
            {
                config.tau = tau;                                        ///< Set the current tau value in the configuration.
                config.noncompliant_tenant_number = noncompliant_number; ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config);            ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                // Run Python scripts for data analysis.
                sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_CHART_PATH, name, configuration_path); ///< Command for average loss chart.
                system(command);                                                                                ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_TAU_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }

        config.noncompliant_tenant_number = 99;                         ///< Set the tenant number for extreme compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_EXTREME_COMPLIANT; ///< Set the mode for extreme compliance.

        // Loop through tau values and execute the main program for each in extreme compliant mode.
        for (long tau = 0; tau <= 25600; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            // Run Python scripts for data analysis.
            sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_CHART_PATH, name, configuration_path); ///< Command for average loss chart.
            system(command);                                                                                ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_TAU_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }
#endif

#ifdef APPPEND_MODE

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.

        config.noncompliant_tenant_number = 1;                             ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_EXTREME_NONCOMPLIANT; ///< Set the mode for extreme non-compliance.

        // Loop through tau values and execute the main program for each.
        for (long tau = 25600 + step; tau <= 51200; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }

        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE; ///< Set the mode for non-compliant configuration to average.

        // Loop through tenant numbers and tau values for further analysis.
        for (int noncompliant_number = 25; noncompliant_number <= 75; noncompliant_number += 25)
            for (long tau = 25600 + step; tau <= 51200; tau += step)
            {
                config.tau = tau;                                        ///< Set the current tau value in the configuration.
                config.noncompliant_tenant_number = noncompliant_number; ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config);            ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.                                                                                             ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }

        config.noncompliant_tenant_number = 99;                         ///< Set the tenant number for extreme compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_EXTREME_COMPLIANT; ///< Set the mode for extreme compliance.

        // Loop through tau values and execute the main program for each in extreme compliant mode.
        for (long tau = 25600 + step; tau <= 51200; tau += step)
        {
            config.tau = tau;                             ///< Set the current tau value in the configuration.
            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }
#endif
        break;
    case BURST_ALL_COMPLIANT:
        strcpy(name, "burst_all_compliant");                          ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 0;         ///< Set the initial tenant number for non-compliant mode.
        config.upper_queue_buffer = 20;

        // Loop through state_r values and tau values, executing the main program for each combination.
        for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step) // Iterate over state_r values.
            for (long tau = 0; tau <= 51200; tau += (step * 8))            // Iterate over tau values.
            {
                config.state_r = state_r;                     ///< Set the current state_r value in the configuration.
                config.tau = tau;                             ///< Set the current tau value in the configuration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                // Run Python scripts for data analysis.
                sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_ALL_CHART_PATH, name, configuration_path); ///< Command for average loss chart.
                system(command);                                                                                    ///< Execute the command.
            }

        break;
    case BURST_NONCOMPLIANT:
        strcpy(name, "burst_half_noncompliant");                      ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 50;        ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;
        config.upper_queue_buffer = 25;

        // Loop through state_r values and tau values, executing the main program for each combination.
        for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step) // Iterate over state_r values.
            for (long tau = 0; tau <= 51200; tau += (step * 8))            // Iterate over tau values.
            {
                config.state_r = state_r; ///< Set the current state_r value in the configuration.
                config.tau = tau;         ///< Set the current tau value in the configuration.

                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.
            }
        break;
    case BURST_NONCOMPLIANT_DIFFERENT_R:
        strcpy(name, "burst_noncompliant_different_r");               ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_DIFFERENT_R; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 50;         ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // Loop through state_r values and tau values, executing the main program for each combination.
        for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step) // Iterate over state_r values.
            for (double noncompliant_state_r = 0.6; noncompliant_state_r < 0.9; noncompliant_state_r += state_r_step)
                for (long tau = 0; tau <= 25600; tau += step) // Iterate over tau values.
                {
                    if (noncompliant_state_r <= state_r)
                        continue;

                    config.noncompliant_state_r = noncompliant_state_r;
                    config.state_r = state_r;                     ///< Set the current state_r value in the configuration.
                    config.tau = tau;                             ///< Set the current tau value in the configuration.
                    modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                    sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                    system(command);                                              ///< Execute the command.

                    sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                    system(command);                                                                                                  ///< Execute the command.
                }
        break;
    case BURST_NONCOMPLIANT_DIFFERENT_FIFO_QUEUE_BUFFER:
        strcpy(name, "burst_half_noncompliant_different_FIFO");       ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        // sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        // system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        // write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        // write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        // write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 50;        ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // Loop through state_r values and tau values, executing the main program for each combination.

        // for (int upper_queue_buffer = 20; upper_queue_buffer <= 100; upper_queue_buffer += step_upper_queu_buffer)
        for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step) // Iterate over state_r values.
            for (long tau = 25600 + step; tau <= 51200; tau += step)       // Iterate over tau values.
            {
                config.upper_queue_buffer = 10;
                config.state_r = state_r;                     ///< Set the current state_r value in the configuration.
                config.tau = tau;                             ///< Set the current tau value in the configuration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.
            }
        break;

    case UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER_FIFO:
        strcpy(name, "uniform_different_noncompliant_number_FIFO");   ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;    ///< Set the mode for non-compliant configuration to average.
        config.noncompliant_tenant_number = 75;
        // Loop through tenant numbers and tau values for further analysis.
        for (int upper_queue_buffer = 6; upper_queue_buffer < 10; upper_queue_buffer++)
            for (long tau = 0; tau <= 51200; tau += (step * 2))
            {
                config.upper_queue_buffer = upper_queue_buffer;
                config.tau = tau;                             ///< Set the current tau value in the configuration.
                                                              ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }

        config.noncompliant_tenant_number = 99;                         ///< Set the tenant number for extreme compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_EXTREME_COMPLIANT; ///< Set the mode for extreme compliance.

        // Loop through tenant numbers and tau values for further analysis.
        for (int upper_queue_buffer = 6; upper_queue_buffer < 10; upper_queue_buffer++)
            for (long tau = 0; tau <= 51200; tau += (step * 2))
            {
                config.upper_queue_buffer = upper_queue_buffer;
                config.tau = tau;                             ///< Set the current tau value in the configuration.
                                                              ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }

        break;
    case BURST_ALL_COMPLIANT_DIFFERENT_FIFO_QUEUE_BUFFER:
        strcpy(name, "burst_all_compliant_different_FIFO");           ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 0;         ///< Set the initial tenant number for non-compliant mode.

        // Loop through state_r values and tau values, executing the main program for each combination.

        for (int upper_queue_buffer = 5; upper_queue_buffer <= 100; upper_queue_buffer += 5)
            for (double state_r = 0.6; state_r < 0.9; state_r += state_r_step) // Iterate over state_r values.
            {
                config.upper_queue_buffer = upper_queue_buffer;
                config.state_r = state_r;                     ///< Set the current state_r value in the configuration.
                config.tau = 25600;                           ///< Set the current tau value in the configuration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                    ///< Execute the command.
            }
        break;
    case BURST_NONCOMPLIANT_SIX_SEVEN:
        strcpy(name, "burst_half_noncompliant_six_seven");            ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        // sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        // config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        // modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // // Prepare the data directory by removing existing data and creating new directories.
        // sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        // system(command);                                       ///< Execute the command.

        // // Write headers for statistics with different types.
        // write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        // write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        // write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 50;        ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // // Loop through state_r values and tau values, executing the main program for each combination.
        // for (double state_r = 0.6; state_r <= 0.7; state_r += 0.025) // Iterate over state_r values.
        //     for (long tau = 0; tau <= 51200; tau += (512 * 4))       // Iterate over tau values.
        //     {
        //         config.state_r = state_r; ///< Set the current state_r value in the configuration.
        //         config.tau = tau;         ///< Set the current tau value in the configuration.

        //         modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

        //         sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
        //         system(command);                                              ///< Execute the command.

        //         sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
        //         system(command);                                                                                                  ///< Execute the command.
        //     }

        // Loop through state_r values and tau values, executing the main program for each combination.

        for (double state_r = 0.6; state_r <= 0.64; state_r += 0.01) // Iterate over state_r values.
            for (long tau = 0; tau <= 51200; tau += (512 * 4))       // Iterate over tau values.
            {
                config.upper_queue_buffer = 36;
                config.state_r = state_r; ///< Set the current state_r value in the configuration.
                config.tau = tau;         ///< Set the current tau value in the configuration.

                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.
            }
        for (long tau = 0; tau < 47104; tau += (512 * 4)) // Iterate over tau values.
        {
            config.upper_queue_buffer = 36;
            config.state_r = 0.65; ///< Set the current state_r value in the configuration.
            config.tau = tau;      ///< Set the current tau value in the configuration.

            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }
        break;
    case BURST_NONCOMPLIANT_7_25:
        strcpy(name, "burst_half_noncompliant_7_25");                 ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.sprintf(command, "rm -r %s", config.data_path); ///< Command to remove the existing data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        // system(command);                                       ///< Execute the command.

        // // Write headers for statistics with different types.
        // write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        // write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        // write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 25;        ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // Loop through state_r values and tau values, executing the main program for each combination.
        for (int upper_queue_buffer = 52; upper_queue_buffer <= 64; upper_queue_buffer += 4)
            for (long tau = 0; tau <= 51200; tau += 512) // Iterate over tau values.
            {
                config.upper_queue_buffer = upper_queue_buffer;
                config.state_r = 0.7; ///< Set the current state_r value in the configuration.
                config.tau = tau;     ///< Set the current tau value in the configuration.

                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);                                              ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.
            }
        break;
    case SENSITIVE_NONCOMPLIANT_NUMBER:
        strcpy(name, "sensitive_noncompliant_number");                ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.sprintf(command, "rm -r %s", config.data_path); ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        // config.noncompliant_tenant_number = 25;        ///< Set the initial tenant number for non-compliant mode.
        config.noncompliant_mode = NONCOMPLIANT_MODE_BEFORE;
        config.upper_queue_buffer = 10;
        config.tau = 51200;

        for (int noncompliant_tenant_number = 50; noncompliant_tenant_number <= 75; noncompliant_tenant_number++) // Iterate over tau values.
        {
            config.noncompliant_tenant_number = noncompliant_tenant_number;

            modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration values.

            sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
            system(command);                                              ///< Execute the command.

            sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
            system(command);                                                                                                  ///< Execute the command.
        }
        break;

    case UNIFORM_DISTRIBUTION_NONCOMPLIANT_DIFFERENT_NONCOMPLIANT_NUMBER_RATIO:
        strcpy(name, "uniform_different_noncompliant_number_ratio");  ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.
        config.noncompliant_mode = NONCOMPLIANT_MODE_BEFORE;     ///< Set the mode for non-compliant configuration to average.

        system("gcc ./advanced_more_capacity.c inih/ini.c -o ../execution/advanced_more_capacity -lm");

        // Loop through tenant numbers and tau values for further analysis.
        for (float ratio = 1.0001; ratio <= 1.001; ratio += 0.0001)
            for (int noncompliant_tenant_number = 60; noncompliant_tenant_number < 100; noncompliant_tenant_number++)
            {
                config.noncompliant_tenant_number = noncompliant_tenant_number;
                config.upper_queue_buffer = 10;
                config.tau = 51200;                           ///< Set the current tau value in the configuration.
                                                              ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/advanced_more_capacity %s %f", configuration_path, ratio); ///< Command to execute the main program.
                system(command);                                                                          ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }

        for (float ratio = 1.0001; ratio <= 1.001; ratio += 0.0001)
            for (int noncompliant_tenant_number = 60; noncompliant_tenant_number < 100; noncompliant_tenant_number++)
            {
                config.noncompliant_tenant_number = noncompliant_tenant_number;
                config.upper_queue_buffer = 10;
                config.tau = 25600;                           ///< Set the current tau value in the configuration.
                                                              ///< Update the tenant number for the current iteration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new configuration.

                sprintf(command, "../execution/advanced_more_capacity %s %f", configuration_path, ratio); ///< Command to execute the main program.
                system(command);                                                                          ///< Execute the command.

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);
            }
        break;
    case UNIFORM_DISTRIBUTION_NONCOMPLIANT_RATIO:
        strcpy(name, "uniform_half_noncompliant_ratio");              ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        config.traffic_mode = TRAFFIC_MODE_UNIFORM;   ///< Set the traffic mode to uniform.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_NONCOMPLIANT_UNIFORM; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_mean = 155;                          ///< Set the mean for non-compliant configuration.
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;    ///< Set the mode for non-compliant configuration.

        // Loop through tau values and execute the main program for each.
        for (double ratio = 1.001; ratio < 1.01; ratio += 0.001)
            for (long tau = 0; tau <= 51200; tau += (step * 2))
            {
                config.tau = tau;
                config.ratio = ratio;                         ///< Set the current tau value in the configuration.
                modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

                sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                system(command);

                sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                system(command);                                                                                                  ///< Execute the command.
            }

        break; ///< End of the case for UNIFORM_DISTRIBUTION_NONCOMPLIANT.
    case BURST_ALL_COMPLIANT_FIFO_RATIO:
        strcpy(name, "burst_all_compliant_FIFO_ratio");               ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        config.traffic_mode = TRAFFIC_MODE_UNIFORM;   ///< Set the traffic mode to uniform.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // Prepare the data directory by removing existing data and creating new directories.
        sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        system(command);                                       ///< Execute the command.
        sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        system(command);                                       ///< Execute the command.

        // Write headers for statistics with different types.
        write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 0;         ///< Set the mean for non-compliant configuration.
        config.tau = 25600;

        // Loop through tau values and execute the main program for each.
        for (double ratio = 1.001; ratio < 1.01; ratio += 0.001)
            for (double state_r = 0.6; state_r < 0.9; state_r += 0.01)
                for (int upper_buffer = 1; upper_buffer <= 100; upper_buffer++)
                {
                    config.state_r = state_r;
                    config.upper_queue_buffer = upper_buffer;
                    config.ratio = ratio;                         ///< Set the current tau value in the configuration.
                    modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

                    sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                    system(command);

                    sprintf(command, "python3 %s %s %s", PYTHON_AVERAGE_LOSS_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                    system(command);                                                                                    ///< Execute the command.
                }

        break; ///< End of the case for UNIFORM_DISTRIBUTION_NONCOMPLIANT.
    case BURST_NONCOMPLIANT_RATIO:
        strcpy(name, "burst_half_noncompliant_ratio");                ///< Set the name for the configuration.
        sprintf(configuration_path, "../configuration/%s.ini", name); ///< Construct the path to the configuration INI file.
        reduction_inif_file(configuration_path);                      ///< Process the INI file to initialize settings.

        // Parse the INI file and load the configuration.
        if (ini_parse(configuration_path, handler, &config) < 0)
        {
            printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path); ///< Error message if loading fails.
            return EXIT_FAILURE;                                                      ///< Exit with failure status if configuration cannot be loaded.
        }

        sprintf(data_path, "../data/%s", name);       ///< Construct the path for the data directory.
        config.data_path = data_path;                 ///< Set the data path in the configuration structure.
        config.traffic_mode = TRAFFIC_MODE_UNIFORM;   ///< Set the traffic mode to uniform.
        modify_ini_file(configuration_path, &config); ///< Update the INI file with the current configuration.

        // // Prepare the data directory by removing existing data and creating new directories.
        // sprintf(command, "rm -r %s", config.data_path);        ///< Command to remove the existing data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s", config.data_path);        ///< Command to create the data directory.
        // system(command);                                       ///< Execute the command.
        // sprintf(command, "mkdir %s/images", config.data_path); ///< Command to create the images subdirectory.
        // system(command);                                       ///< Execute the command.

        // // Write headers for statistics with different types.
        // write_statistics_header_config(config, HEADER_TYPE_AVERAGE); ///< Write the header for average statistics.
        // write_statistics_header_config(config, HEADER_TYPE_ALL);     ///< Write the header for all statistics.
        // write_statistics_header_config(config, HEADER_TYPE_TAU);     ///< Write the header for tau statistics.

        // Set additional configuration parameters for non-compliant mode.
        // config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        // config.noncompliant_tenant_number = 50;        ///< Set the mean for non-compliant configuration.
        // config.noncompliant_mean = 155;
        // config.upper_queue_buffer = 25;
        // config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // // Loop through tau values and execute the main program for each.
        // for (double ratio = 1.001; ratio < 1.01; ratio += 0.001)
        //     for (double state_r = 0.6; state_r < 0.9; state_r += 0.1)
        //         for (long tau = 0; tau <= 51200; tau += (step * 4))
        //         {
        //             config.state_r = state_r;
        //             config.tau = tau;
        //             config.ratio = ratio;                         ///< Set the current tau value in the configuration.
        //             modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

        //             sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
        //             system(command);

        //             sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
        //             system(command);                                                                                                  ///< Execute the command.
        //         }

        config.traffic_mode = TRAFFIC_MODE_BURSTY_ALL; ///< Update traffic mode to non-compliant uniform.
        config.noncompliant_tenant_number = 50;        ///< Set the mean for non-compliant configuration.
        config.noncompliant_mean = 155;
        config.upper_queue_buffer = 50;
        config.noncompliant_mode = NONCOMPLIANT_MODE_AVERAGE;

        // Loop through tau values and execute the main program for each.
        for (double ratio = 1.001; ratio < 1.01; ratio += 0.001)
            for (double state_r = 0.8; state_r < 0.9; state_r += 0.1)
                for (long tau = 0; tau <= 51200; tau += (step * 4))
                {
                    config.state_r = state_r;
                    config.tau = tau;
                    config.ratio = ratio;                         ///< Set the current tau value in the configuration.
                    modify_ini_file(configuration_path, &config); ///< Update the INI file with the new tau value.

                    sprintf(command, "../execution/main %s", configuration_path); ///< Command to execute the main program.
                    system(command);

                    sprintf(command, "python3 %s %s %s", PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH, name, configuration_path); ///< Command for compliant and non-compliant tau chart.
                    system(command);                                                                                                  ///< Execute the command.
                }

        break; ///< End of the case for UNIFORM_DISTRIBUTION_NONCOMPLIANT.
    default:
        printf(RED_ELOG "CODE DOES NOT EXIST!\n" RESET); ///< Error message for unsupported configuration.
        return EXIT_FAILURE;                             ///< Exit with failure status for unknown configurations.
    }
    return EXIT_SUCCESS;
}