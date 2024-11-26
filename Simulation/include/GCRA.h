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

void record_gcras(GCRA* gcras, int tenant_number,const char* folder_path, int type){
    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%s/GCRA_%d.csv", folder_path, type);
    FILE* file = fopen(data_path, "w+");
    if (!file) {
        perror("Error opening file");
        return;
    }

    for(int tenant = 0;tenant<tenant_number;tenant++){
        GCRA* pgcra = (GCRA*)(gcras+tenant);
        fprintf(file, "%ld, %ld\n", pgcra->x, pgcra->last_time);
    }

    fclose(file);
}

void read_gcras(GCRA** pgcras, int tenant_number,const char* folder_path, int type){
    char data_path[MAX_PATH_LENGTH];
    sprintf(data_path, "%s/GCRA_%d.csv", folder_path, type);
    FILE* file = fopen(data_path, "r");
    if (!file) {
        perror("Error opening file");
        return;
    }

    // Read and parse each line
    char line[MAX_BUFFER_SIZE];
    size_t row = 0;
    while (fgets(line, sizeof(line), file) && row < tenant_number) {
        GCRA* pgcra = (GCRA*)(*(pgcras)+row);
        long x, last_time;
        if (sscanf(line, "%ld, %ld", &x, &last_time) == 2) {
            pgcra->x = x;
            pgcra->last_time = last_time;
            row++;
        } else {
            fprintf(stderr, "Error parsing line: %s\n", line);
        }
    }

    fclose(file);
}
#endif

// printGCRA(*(gcras));