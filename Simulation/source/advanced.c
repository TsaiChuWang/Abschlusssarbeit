#define PRINT_EXECUTION_TIME ///< Enable timing measurement for execution duration
// #define SHOW_CONFIGURATION   ///< Enable display of the current configuration settings

#include "../include/general.h"       ///< Include general definitions and declarations.
#include "./inih/ini.h"               ///< Include INI file handling library.
#include "../include/configuration.h" ///< Include configuration management definitions.

#define CONFIGURATION_PATH "../configuration/advanced.ini" ///< Path to the main configuration file
#define CSV_PATH "../configuration/csv/advanced.csv"       ///< Path to the main csv file

/** @brief Compilation command
 *  @code{.sh}
 *  gcc ./advanced.c inih/ini.c -o ../execution/advanced -lm
 *  @endcode
 */

/** @brief Execution command
 *  @code{.sh}
 *  ../execution/advanced
 *  @endcode
 */

int main(int argc, char *argv[])
{
    char configuration_path[MAX_PATH_LENGTH];
    // Check if a configuration path is provided as a command-line argument
    if (argc < 2)
        strcpy(configuration_path, CONFIGURATION_PATH); // Use default configuration path
    else
        strcpy(configuration_path, argv[1]); // Use the provided configuration path

#ifdef PRINT_EXECUTION_TIME
    clock_t execute_clock = clock(); ///< Start time measurement for performance tracking
#endif

    common_configuration config; ///< Configuration structure to hold settings

    /** @brief Load and parse the configuration file.
     *
     * This function reads the specified INI file and populates the
     * configuration structure. If the file cannot be loaded or parsed,
     * an error message is displayed and the program exits.
     */
    if (ini_parse(configuration_path, handler_common_configuration, &config) < 0)
    {
        printf(RED_ELOG "Can't load configuration \"%s\"\n", configuration_path);
        return EXIT_FAILURE;
    }

#ifdef SHOW_CONFIGURATION
    show_configuration_common_configuration(config); /** @brief Display current configuration if enabled. */
#endif
    test_configuration_h_common_configuration(configuration_path);
    test_csv_function(CSV_PATH, &config);

#ifdef PRINT_EXECUTION_TIME
    execute_clock = clock() - execute_clock;                      ///< Calculate the elapsed execution time.
    double time_taken = ((double)execute_clock) / CLOCKS_PER_SEC; ///< Convert clock ticks to seconds.
    printf("Execute time : %f s\n", time_taken);                  ///< Print the execution time in seconds.
#endif
    return EXIT_SUCCESS;
}