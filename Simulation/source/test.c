#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum { STATE_NO_PACKET, STATE_CREATE_PACKET } state_t;

typedef struct {
  state_t state;
  double p;  // Probability of staying in STATE_NO_PACKET
  double r;  // Probability of staying in STATE_CREATE_PACKET
} state_machine;

void change_state(state_machine* pstate) {
  double factor = drand48();  // 產生 0~1 之間的隨機數
  
  if (pstate->state == STATE_CREATE_PACKET) {
    if (factor < pstate->r) {
      pstate->state = STATE_CREATE_PACKET;
    } else {
      pstate->state = STATE_NO_PACKET;
    }
  } else {
    if (factor < pstate->p) {
      pstate->state = STATE_NO_PACKET;
    } else {
      pstate->state = STATE_CREATE_PACKET;
    }
  }
}

int main() {
  srand48(time(NULL));  // 只初始化一次隨機種子
  
  state_machine sm = {STATE_NO_PACKET, 0.875, 0.982};

  // **預跑 50 萬次，確保達到穩態**
  for (int i = 0; i < 500000; i++) {
    change_state(&sm);
  }

  int count = 0;
  int total_steps = 327680;

  for (int i = 0; i < total_steps; i++) {
    change_state(&sm);
    if (sm.state == STATE_CREATE_PACKET) {
      count++;
    }
  }

  for (int i = 1; i <= 6; i++) {
    printf("if(a == '%.f' and rn == 'r'):\n", (0.9+(double)i*0.0125)*10000);
    printf("\tr%.f = sum_chunks(r%.f, 10)\n", (0.9+(double)i*0.0125)*10000, (0.9+(double)i*0.0125)*10000);
    printf("\tx = [(i+1)*3051 for i in range(len(r%.f))]\n", (0.9+(double)i*0.0125)*10000, (0.9+(double)i*0.0125)*10000);
    printf("\tplt.plot(x, r%.f, linestyle='-', label='regular')\n", (0.9+(double)i*0.0125)*10000);
    printf("\tplt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='preficted mean')\n");
    printf("\tplt.title('State Machine r= %.4f Regular')\n\n", (0.9+(double)i*0.0125));

    printf("if(a == '%.f' and rn == 'n'):\n", (0.9+(double)i*0.0125)*10000);
    printf("\tn%.f = sum_chunks(n%.f, 10)\n", (0.9+(double)i*0.0125)*10000, (0.9+(double)i*0.0125)*10000);
    printf("\tx = [(i+1)*3051 for i in range(len(n%.f))]\n", (0.9+(double)i*0.0125)*10000, (0.9+(double)i*0.0125)*10000);
    printf("\tplt.plot(x, n%.f, linestyle='-', label='naughty')\n", (0.9+(double)i*0.0125)*10000);
    printf("\tplt.plot(x, [0.75 for i in x], linestyle='-', color = 'red', label='preficted mean')\n");
    printf("\tplt.title('State Machine r= %.4f Naughty')\n", (0.9+(double)i*0.0125));
  }
  printf("\n");

  // for (int i = 1; i <= 8; i++) {
  //   printf("if((states_%d)->state == STATE_CREATE_PACKET){\n", i);
  //   printf("\tfprintf(file, \"1,\");\n");
  //   printf("\t*(arr+%d) += 1;\n", i*2);  
  //   printf("}else fprintf(file, \"0,\");\n");
  //   printf("if((states_%d+1)->state == STATE_CREATE_PACKET){\n", i);
  //   printf("\tfprintf(file, \"1,\");\n");
  //   printf("\t*(arr+%d) += 1;\n", i*2+1);  
  //   printf("}else fprintf(file, \"0,\");\n"); 
  //   printf("\n");
  // }
  

  printf("STATE_CREATE_PACKET count: %d (%.2f%%)\n", count, count / (double)total_steps * 100);
  return 0;
}
