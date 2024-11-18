
#include <pthread.h>
#include <unistd.h>  // For sleep()

#include "../Enthalten/include.h"
#define _NET_NAMESPCE
#include "../Enthalten/netnamespce.h"

// gcc simple_V7_1.c -o ../Ausführung/simple_V7_1
// ../Ausführung/simple_V7_1

int main(int argc, char *argv[]){

  int simluation_time = 10;

  NetworkNamesapce destination = announceNetworkNamesapce_general(1);
  NetworkNamesapce source_1 = announceNetworkNamesapce_general(2);
  // informNetworkNamesapce(destination);

  Veth veth0 = announceVeth_general(0, source_1, destination);
  Veth veth1 = announceVeth_general(1, destination, source_1);
  // informVeth(veth0);
  // informVeth(veth1);

  Bridge bridge = announceBridge_general(0, destination);
  // informBridge(bridge);

  createNetworkNamesapce(&destination);
  createNetworkNamesapce(&source_1);

  createVethPair(&veth0, &veth1);

  createBridge(&bridge);
  addVethtoBridge(&bridge, &veth1);

  configureIPaddressNetworkNamesapce_bridge(&destination, &bridge);
  configureIPaddressNetworkNamesapce_veth(&source_1, &veth0);

  // system("sudo ip netns exec ns2 ping 10.0.0.1 -c 5");
  // system("sudo ip netns list");

  pthread_t thread_receive, thread_send;

  
  // executeReceiveProgram(&destination);
  fflush(stdout);
  if(pthread_create(&thread_receive, NULL, receiveThread, &destination)) {
    // fprintf(stderr, "Error creating thread 1\n");
    printf("Error creating thread 1\n");
    return EXIT_FAILURE;
  }
  ThreadArgument_Speicified_Packet_Number_Per_Second thread_arguments;
  thread_arguments.net_namespace = &source_1;
  thread_arguments.packet_number_persecond = 20;
  fflush(stdout);
  if (pthread_create(&thread_send, NULL, sendThreadSpeiciedPersencond, &thread_arguments)) {
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

  // deleteVeth(&veth0);
  // deleteVeth(&veth1);
  return EXIT_SUCCESS;
}
