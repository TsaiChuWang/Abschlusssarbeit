#define REDUCTION

#include "../include/general.h"             ///< Include general definitions and declarations.
#include "./inih/ini.h"                     ///< Include INI file handling library.
#include "../include/configuration.h"       ///< Include configuration management definitions.

#define CONFIGURATION_PATH "../configuration/advanced.ini" ///< Path to the main configuration file

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
    if (argc < 2)
        strcpy(configuration_path, CONFIGURATION_PATH);
    else
        strcpy(configuration_path, argv[1]);

    test_configuration_h_common_configuration(CONFIGURATION_PATH);

    return EXIT_SUCCESS;
}