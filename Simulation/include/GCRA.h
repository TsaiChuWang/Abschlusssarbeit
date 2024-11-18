#define GCRA_H

// #define PACKET_LABEL_ACCEPT 0
// #define PACKET_LABEL_OVER_UPPERBOUND_DROPPED 1
// #define PACKET_LABEL_GCRA_DROPPED 2
// #define PACKET_LABEL_OVER_CAPACITY_DROPPED 3

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef GCRA_H

typedef struct{
    long tau;    
    long l;   
    long last_time;

    long x; // water level
}GCRA;

GCRA initializeGCRA(long tau, long l){
    GCRA gcra;

    gcra.tau = tau;
    gcra.l = l;
    gcra.last_time = 0;
    gcra.x = 0;

    return gcra;
}

GCRA* initializeGCRAs(int tenant_number, long tau, long l){
    GCRA* gcras = (GCRA*)malloc(sizeof(GCRA)*tenant_number);
    for(int index=0;index<tenant_number;index++)
        *(gcras+index) = initializeGCRA(tau, l);
    return gcras;
}

void printGCRA(GCRA gcra){
    printf("tau       = %-ld\n", gcra.tau);
    printf("l         = %-ld\n", gcra.l);
    printf("last time = %-ld\n", gcra.last_time);
    printf("x         = %-ld\n", gcra.x);
}

int first_gcra_limit(long timestamp, GCRA* pgcra, long upper_bound, int packet_size, long temp_tau){
    long x = pgcra->x - (timestamp - pgcra->last_time)*(((int)upper_bound*packet_size)/ONE_SECOND_IN_NS);
    
    // printf("%ld %ld %ld\n", pgcra->x, timestamp, (timestamp - pgcra->last_time)*(((int)upper_bound*packet_size)/ONE_SECOND_IN_NS));
    if(x > temp_tau)
        return PACKET_LABEL_OVER_UPPERBOUND_DROPPED;
    else{
        pgcra->x = MAX((long)0, x)+pgcra->l;
        pgcra->last_time = timestamp;
        return PACKET_LABEL_ACCEPT;
    }
}

void refresh_gcra(GCRA* pgcra){
    pgcra->last_time = 0;
    pgcra->x = 0;
}

int second_gcra_limit(int label, long timestamp, GCRA* pgcra, long mean, int packet_size, long tau){
    long x = pgcra->x - (timestamp - pgcra->last_time)*(((int)mean*packet_size)/ONE_SECOND_IN_NS);
    // printf("%ld %ld %ld\n", pgcra->x, timestamp, (timestamp - pgcra->last_time)*(((int)upper_bound*packet_size)/ONE_SECOND_IN_NS));
    
    if(label == PACKET_LABEL_OVER_UPPERBOUND_DROPPED)
        return PACKET_LABEL_OVER_UPPERBOUND_DROPPED;

    if(x > tau)
        return PACKET_LABEL_GCRA_DROPPED;
    else{
        pgcra->x = MAX((long)0, x)+pgcra->l;
        pgcra->last_time = timestamp;
        return PACKET_LABEL_ACCEPT;
    }
}

#endif

// printGCRA(*(gcras));