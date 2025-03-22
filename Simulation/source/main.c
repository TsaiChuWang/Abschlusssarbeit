
// #define REDUCTION
#define RECORD_PACKETS_GENERATION
#define RECORD_TIMESTAMP

#include "../include/general.h"
#include "./inih/ini.h"
#include "../include/configuration.h"
#include "../include/state_machine.h"
#include "../include/traffic_generation.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/GCRA.h"
// #include "../include/link_capacity_queue.h"
// #include "../include/packets_count.h"

#define CONFIGURATION_PATH "../configuration/main.ini"

#define NAME "main"

// gcc ./main.c inih/ini.c -o ../execution/main -lm
// ../execution/main [traffic_generation_code]

#define GENERATE_TRAFFIC 1

state_t dice(double threshold) {
    double factor = drand48();  // 產生 0~1 之間的隨機數
    
    // printf("%f\n", factor);
      if (factor < threshold) {
        return STATE_CREATE_PACKET;
      } else {
        return STATE_NO_PACKET;
      }
    
  }

int main(int argc, char *argv[])
{
    char command[MAX_COMMAND_LENGTH];

    // configuration.h
    configuration config;

    if (ini_parse(CONFIGURATION_PATH, handler, &config) < 0)
    {
        printf("Can't load configuration \"%s\"\n", CONFIGURATION_PATH);
        return EXIT_FAILURE;
    }
    // show_configuration(config);


    srand48(time(NULL));

    // system("rm ../data/naughty.csv");
    FILE *file = fopen("../data/traffic.csv", "w+");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(file, "ru,nu,r0.875,n0.875,r0.9,n0.9,r0.9125,n0.9125,r0.925,n0.925,r0.9375,n0.9375,r0.95,n0.95,r0.9625,n0.9625,r0.975,n0.975,\n");
    // fclose(file);

    state_machine* states_1 = initialize_state_machines(0.875, 0.75, 0.875, 2, 1);
    state_machine* states_2 = initialize_state_machines(0.9, 0.75, 0.875, 2, 1);
    state_machine* states_3 = initialize_state_machines(0.9125, 0.75, 0.875, 2, 1);
    state_machine* states_4 = initialize_state_machines(0.925, 0.75, 0.875, 2, 1);
    state_machine* states_5 = initialize_state_machines(0.9375, 0.75, 0.875, 2, 1);
    state_machine* states_6 = initialize_state_machines(0.95, 0.75, 0.875, 2, 1);
    state_machine* states_7 = initialize_state_machines(0.9625, 0.75, 0.875, 2, 1);
    state_machine* states_8 = initialize_state_machines(0.975, 0.75, 0.875, 2, 1);

    int* arr = calloc(18, sizeof(int));
    for(int i=0;i<327680;i++){

        if(dice(0.75) == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *arr += 1;
        }else fprintf(file, "0,");

        if(dice(0.875) == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+1) += 1;
        }else fprintf(file, "0,");

        change_states(states_1, 2);
        change_states(states_2, 2);
        change_states(states_3, 2);
        change_states(states_4, 2);
        change_states(states_5, 2);
        change_states(states_6, 2);
        change_states(states_7, 2);
        change_states(states_8, 2);

        if((states_1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+2) += 1;
        }else fprintf(file, "0,");
        if((states_1+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+3) += 1;
        }else fprintf(file, "0,");
        
        if((states_2)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+4) += 1;
        }else fprintf(file, "0,");
        if((states_2+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+5) += 1;
        }else fprintf(file, "0,");
        
        if((states_3)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+6) += 1;
        }else fprintf(file, "0,");
        if((states_3+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+7) += 1;
        }else fprintf(file, "0,");
        
        if((states_4)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+8) += 1;
        }else fprintf(file, "0,");
        if((states_4+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+9) += 1;
        }else fprintf(file, "0,");
        
        if((states_5)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+10) += 1;
        }else fprintf(file, "0,");
        if((states_5+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+11) += 1;
        }else fprintf(file, "0,");
        
        if((states_6)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+12) += 1;
        }else fprintf(file, "0,");
        if((states_6+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+13) += 1;
        }else fprintf(file, "0,");
        
        if((states_7)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+14) += 1;
        }else fprintf(file, "0,");
        if((states_7+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+15) += 1;
        }else fprintf(file, "0,");
        
        if((states_8)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,");
            *(arr+16) += 1;
        }else fprintf(file, "0,");
        if((states_8+1)->state == STATE_CREATE_PACKET){
            fprintf(file, "1,\n");
            *(arr+17) += 1;
        }else fprintf(file, "0\n");
        
    }fclose(file);

    for(int j=0;j<18;j++)
        printf("%d (%f %)\n", *(arr+j), (double)*(arr+j)/327680);

    return EXIT_SUCCESS;
}