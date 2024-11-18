
#include <pthread.h>
#include <unistd.h>  // For sleep()

#include "../Enthalten/include.h"
#define _NET_NAMESPCE
#include "../Enthalten/netnamespce.h"

// gcc simple_V7_3.c -o ../Ausführung/simple_V7_3
// ../Ausführung/simple_V7_3

int main(int argc, char *argv[]){

  int simluation_time = 25;

  NetworkNamesapce destination = announceNetworkNamesapce_general(1);
  NetworkNamesapce source_1 = announceNetworkNamesapce_general(2);
  NetworkNamesapce source_2 = announceNetworkNamesapce_general(3);
  // informNetworkNamesapce(source_2);

  Veth veth0 = announceVeth_general(0, source_1, destination);
  Veth veth1 = announceVeth_general(1, destination, source_1);

  Veth veth2 = announceVeth_general(2, source_2, destination);
  Veth veth3 = announceVeth_general(3, destination, source_2);
  // informVeth(veth0);
  // informVeth(veth1);

  Bridge bridge = announceBridge_general(0, destination);
  // informBridge(bridge);

  createNetworkNamesapce(&destination);
  createNetworkNamesapce(&source_1);
  createNetworkNamesapce(&source_2);

  createVethPair(&veth0, &veth1);
  createVethPair(&veth2, &veth3);

  createBridge(&bridge);
  addVethtoBridge(&bridge, &veth1);
  addVethtoBridge(&bridge, &veth3);

  configureIPaddressNetworkNamesapce_bridge(&destination, &bridge);
  configureIPaddressNetworkNamesapce_veth(&source_1, &veth0);
  configureIPaddressNetworkNamesapce_veth(&source_2, &veth2);

  // system("sudo ip netns exec ns3 ping 10.0.0.1 -c 3");
  // system("sudo ip netns list");

  pthread_t thread_receive, thread_send_1, thread_send_2;

  
  // executeReceiveProgram(&destination);
  fflush(stdout);
  if(pthread_create(&thread_receive, NULL, receiveThread, &destination)) {
    fflush(stdout);
    fprintf(stderr, "Error creating thread 1\n");
    printf("Error creating thread 1\n");
    return EXIT_FAILURE;
  }
  ThreadArgument_Trafffic_Generation thread_arguments_1;
  thread_arguments_1.net_namespace = &source_1;
  thread_arguments_1.simulation_time = simluation_time-5;
  fflush(stdout);
  if (pthread_create(&thread_send_1, NULL, sendThreadTrafficGeneration_Multiple, &thread_arguments_1)) {
      fprintf(stderr, "Error creating thread 2\n");
      printf("Error creating thread 2\n");
      return EXIT_FAILURE;
  }

  // system("sudo ip netns exec ns3 iperf -c 10.0.0.1 -u -b 409.6k -l 256 -t 1");

  ThreadArgument_Trafffic_Generation thread_arguments_2;
  thread_arguments_2.net_namespace = &source_2;
  thread_arguments_2.simulation_time = simluation_time-5;
  fflush(stdout);
  if (pthread_create(&thread_send_2, NULL, sendThreadTrafficGeneration_Multiple, &thread_arguments_2)) {
      fprintf(stderr, "Error creating thread 2\n");
      printf("Error creating thread 2\n");
      return EXIT_FAILURE;
  }

  sleep(simluation_time);

  // pthread_cancel(thread_receive);
  // printf("Thread has been canceled\n");
  // pthread_join(thread_receive, NULL);


  deleteNetworkNamesapce(&destination);
  deleteNetworkNamesapce(&source_1);
  deleteNetworkNamesapce(&source_2);

  // deleteVeth(&veth0);
  // deleteVeth(&veth1);
  return EXIT_SUCCESS;
}
