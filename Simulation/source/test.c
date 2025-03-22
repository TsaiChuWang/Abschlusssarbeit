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

  printf("STATE_CREATE_PACKET count: %d (%.2f%%)\n", count, count / (double)total_steps * 100);
  return 0;
}
