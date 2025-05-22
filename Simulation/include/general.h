#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

/**
 * @brief Defines the success status code.
 */
#define SUCCESS 0
/**
 * @brief Defines the status code for an unfound item.
 */
#define UNFOUND -1
/**
 * @brief Defines the comparison result for true.
 */
#define COMPARE_TRUE 0
/**
 * @brief Defines the failure status code.
 */
#define FAILURE -1

/**
 * @brief Defines the boolean value for true.
 */
#define TRUE 1
/**
 * @brief Defines the boolean value for false.
 */
#define FALSE -1

/**
 * @brief Format for identifying an identifier with leading zeros.
 */
#define INFORM_IDENTIFIER_FORMAT "%03u"
/**
 * @brief Format for displaying traffic data with 7 decimal places.
 */
#define INFORM_TRAFFIC_FORMAT "%12.7lf"
/**
 * @brief Format for displaying double precision floating-point numbers with 7 decimal places.
 */
#define INFORM_DOUBLE_FORMAT "%12.7lf"
/**
 * @brief Format for displaying time as a 6-digit integer.
 */
#define INFORM_TIME_FORMAT "%6d"

/**
 * @brief Maximum length for command input.
 */
#define MAX_COMMAND_LENGTH 1024
/**
 * @brief Maximum length for names.
 */
#define MAX_NAME_LENGTH 512
/**
 * @brief Maximum length for file paths.
 */
#define MAX_PATH_LENGTH 2048
/**
 * @brief Maximum size for buffer allocations.
 */
#define MAX_BUFFER_SIZE 4096

#define INFORM_IP_ADDRESS_FORMAT "%s"
#define INFORM_PORT_FORMAT "%d"
#define INFORM_PACKET_SIZE_FORMAT "%d"
#define INFORM_NS_NAME_FORMAT "%s"
#define INFORM_VETH_NAME_FORMAT "%s"
#define INFORM_BRIDGE_NAME_FORMAT "%s"

/**
 * @def ONE_SECOND_IN_NS
 * @brief Conversion constant from seconds to nanoseconds
 * @details Used to convert simulation time units
 */
#define ONE_SECOND_IN_NS 1000000000L
#define TIME_TYPE double

#define PACKET_LABEL_TYPE 4
#define PACKET_LABEL_NO_PACKET -1
#define PACKET_LABEL_ACCEPT 0
#define PACKET_LABEL_OVER_UPPERBOUND_DROPPED 1
#define PACKET_LABEL_GCRA_LABELED 2
#define PACKET_LABEL_OVER_CAPACITY_DROPPED 3

#define PYTHON_CAPACITY_CALCULATION_PATH "../python/capacity.py"
#define CAPACITY_DATA_PATH "../data/capacity.txt"

#define TENANT_TRAFFIC_MODE_INTERVAL 0
#define TENANT_TRAFFIC_MODE_GAUSSIAN 1
#define TENANT_TRAFFIC_MODE_ALL_NAUGHTY 2

#define GBPS 1073741824
#define MBPS 1048576
#define KBPS 1024

#define RED_ELOG "\x1B[1;31m"
#define RESET "\x1B[0m"

#define GENERAL_H
#ifdef GENERAL_H
void print_equals_line()
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        perror("Error getting terminal size");
        return;
    }

    int width = w.ws_col; // Get the terminal's width
    for (int i = 0; i < width; i++)
    {
        putchar('=');
    }
    putchar('\n'); // Add a newline at the end
}

#define GENERATE_BINARY (int)(rand() % 2) /* @brief Generate a binary random value (0 or 1) */

/**
 * @def MAX(a, b)
 * @brief Macro that returns the larger of two values
 *
 * @details Compares two values using the '>' operator and returns
 *          the larger value through a ternary conditional operator.
 *          The macro is type-independent and can be used with any
 *          comparable data types.
 *
 * @param[in] a First value to compare
 * @param[in] b Second value to compare
 *
 * @return The larger value between a and b
 *
 * @note All parameters are parenthesized to prevent operator precedence issues
 *
 * Usage Examples:
 * @code
 * int max_int = MAX(5, 3);           // Returns 5
 * float max_float = MAX(3.14, 2.71); // Returns 3.14
 * double max_double = MAX(x + y, z); // Safe with expressions
 * @endcode
 *
 * @warning
 * - Arguments are evaluated twice, which may cause issues with expressions
 *   that have side effects
 * - Care should be taken when using with complex expressions or function calls
 *
 * Implementation Details:
 * - Uses parentheses around parameters to maintain proper operator precedence
 * - Uses ternary operator for concise comparison
 * - Safe for use with basic arithmetic expressions
 *
 * @see MIN(a, b) for minimum value comparison
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#endif