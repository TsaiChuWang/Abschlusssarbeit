
#define _TRAFFIC_GENERATION_H

#ifdef _TRAFFIC_GENERATION_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MEAN 120
#define STANDARD_DEVIATION 40

int* generateNormalDistribution(int simulation_time, int mean, int standard_deviation, int identifier) {
  srand((unsigned int)time(NULL) + identifier);
  int* traffic = (int*)malloc(sizeof(int)*simulation_time);
  int sum = 0;
  for(int time_stamp = 0;time_stamp<simulation_time;time_stamp++){
    // if(time_stamp == 0 || time_stamp== simulation_time-1)
    //  *(traffic+time_stamp) = (rand() % (2*standard_deviation + 1)) + mean - standard_deviation;
    // else *(traffic+time_stamp) = 0;
    

    *(traffic+time_stamp) = (rand() % (2*standard_deviation + 1)) + mean - standard_deviation;
sum+= *(traffic+time_stamp);
    // printf("%d\n", *(traffic+time_stamp));
  }
  printf("dum = %d\n", sum);
    
  return traffic;
}
#endif