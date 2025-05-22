#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

/** @brief Defines the success status code. */
#define SUCCESS 0
/** @brief Defines the status code for an unfound item.*/
#define UNFOUND -1
/** @brief Defines the comparison result for true. */
#define COMPARE_TRUE 0
/** @brief Defines the failure status code. */
#define FAILURE -1

/** @brief Defines the boolean value for true. */
#define TRUE 1
/** @brief Defines the boolean value for false. */
#define FALSE -1

/** @brief Format for identifying an identifier with leading zeros. */
#define INFORM_IDENTIFIER_FORMAT "%03u"
/** @brief Format for displaying traffic data with 7 decimal places. */
#define INFORM_TRAFFIC_FORMAT "%12.7lf"
/** @brief Format for displaying double precision floating-point numbers with 7 decimal places. */
#define INFORM_DOUBLE_FORMAT "%12.7lf"
/** @brief Format for displaying time as a 6-digit integer. */
#define INFORM_TIME_FORMAT "%6d"

/** @brief Maximum length for command input. */
#define MAX_COMMAND_LENGTH 1024
/** @brief Maximum length for names. */
#define MAX_NAME_LENGTH 512
/** @brief Maximum length for file paths. */
#define MAX_PATH_LENGTH 2048
/** @brief Maximum size for buffer allocations. */
#define MAX_BUFFER_SIZE 4096

/** @brief Format for displaying an IP address as a string. */
#define INFORM_IP_ADDRESS_FORMAT "%s"
/** @brief Format for displaying a port number as an integer. */
#define INFORM_PORT_FORMAT "%d"
/** @brief Format for displaying packet size as an integer. */
#define INFORM_PACKET_SIZE_FORMAT "%d"
/** @brief Format for displaying a nameserver name as a string. */
#define INFORM_NS_NAME_FORMAT "%s"
/** @brief Format for displaying a virtual Ethernet interface name as a string. */
#define INFORM_VETH_NAME_FORMAT "%s"
/** @brief Format for displaying a bridge name as a string. */
#define INFORM_BRIDGE_NAME_FORMAT "%s"

/** @brief Number of nanoseconds in one second. */
#define ONE_SECOND_IN_NS 1000000000L
/** @brief Defines the type used for representing time. */
#define TIME_TYPE double

/** @brief Number of labels for packet classification. */
#define PACKET_LABEL_TYPE_NUMBER 4
/** @brief Label indicating no packet is present. */
#define PACKET_LABEL_NO_PACKET -1
/** @brief Label indicating a packet is accepted. */
#define PACKET_LABEL_ACCEPT 0
/** @brief Label indicating a packet was dropped due to exceeding the upper bound. */
#define PACKET_LABEL_OVER_UPPERBOUND_DROPPED 1
/** @brief Label indicating a packet was labeled by GCRA (Generic Cell Rate Algorithm). */
#define PACKET_LABEL_GCRA_LABELED 2
/** @brief Label indicating a packet was dropped due to over capacity. */
#define PACKET_LABEL_OVER_CAPACITY_DROPPED 3

/** @brief Path to the Python script for capacity calculation. */
#define PYTHON_CAPACITY_CALCULATION_PATH "../python/capacity.py"
/** @brief Path to the Python script for generating the average loss for all charts. */
#define PYTHON_AVERAGE_LOSS_ALL_CHART_PATH "../python/average_loss_all.py"
/** @brief Path to the Python script for generating the average loss chart. */
#define PYTHON_AVERAGE_LOSS_CHART_PATH "../python/average_loss.py"
/** @brief Path to the Python script for generating the packet situation chart. */
#define PYTHON_PACKET_SITUATION_CHART_PATH "../python/packet_situation.py"
/** @brief Path to the Python script for generating compliant and non-compliant charts. */
#define PYTHON_COMPLIANT_AND_NONCOMPLIANT_ALL_CHART_PATH "../python/compliant_and_non_compliant_all.py"
/** @brief Path to the Python script for generating compliant and non-compliant tau charts. */
#define PYTHON_COMPLIANT_AND_NONCOMPLIANT_TAU_CHART_PATH "../python/compliant_and_non_compliant_tau.py"
/** @brief Path to the capacity data file. */
#define CAPACITY_DATA_PATH "../data/capacity.txt"

/** @brief Number of bytes in one gigabit per second (GBps). */
#define GBPS 1073741824
/** @brief Number of bytes in one megabit per second (MBps). */
#define MBPS 1048576
/** @brief Number of bytes in one kilobit per second (KBps). */
#define KBPS 1024

/** @brief ANSI escape code for red text logging. */
#define RED_ELOG "\x1B[1;31m"
/** @brief ANSI escape code to reset text formatting. */
#define RESET "\x1B[0m"

#define GENERAL_H
#ifdef GENERAL_H

/**
 * @brief Prints a line of equals signs across the terminal width.
 */
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

/**
 * @brief Generates a binary random value (0 or 1).
 */
#define GENERATE_BINARY (int)(rand() % 2)

/**
 * @brief Macro to return the maximum of two values.
 *
 * @param a First value.
 * @param b Second value.
 * @return The maximum of a and b.
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @brief Computes the greatest common divisor (GCD) of two integers.
 *
 * @param a First integer.
 * @param b Second integer.
 * @return The GCD of a and b.
 */
int gcd(int a, int b)
{
    while (b != 0)
    {
        /** @brief Temporary variable to store b during swap */
        int temp = b;
        /** @brief Update b with remainder of a/b */
        b = a % b;
        /** @brief Update a with previous value of b */
        a = temp;
    }
    return a;
}

#endif
