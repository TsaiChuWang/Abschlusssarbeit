
#include <pthread.h>
#include <unistd.h>  // For sleep()

#include "../Enthalten/include.h"
#define _NET_NAMESPCE
#include "../Enthalten/netnamespce.h"

// gcc test.c -o ../Ausführung/test
// ../Ausführung/test

int main(int argc, char *argv[]){

  int simluation_time = 25;
  int tenant_number = 2;

  NetworkNamesapce destination = announceNetworkNamesapce_general(1);
  NetworkNamesapce source_1 = announceNetworkNamesapce_general(2);

  FILE* data = fopen("./test.sh", "w+");
    int* traffic = generateNormalDistribution(simluation_time, MEAN, STANDARD_DEVIATION, source_1.identifier);
  for(int time_stamp = 0;time_stamp<simluation_time;time_stamp++){
    char command[MAX_COMMAND_LENGTH];
    sprintf(command, "sudo ip netns exec "INFORM_NS_NAME_FORMAT" iperf -c "DESTINATION_IP" -u -b %ld -l %d -t 1\n", source_1.name, PACKET_SIZE*(*(traffic+time_stamp))*8 ,PACKET_SIZE);
    // system(command);
    fprintf(data, "%s", command);
  }
  fclose(data);
  return EXIT_SUCCESS;
}
