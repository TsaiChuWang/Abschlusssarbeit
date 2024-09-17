#define EVENT


#ifdef DISCRETEEVENT

struct DiscreteEventTenant{
    double traffic;
    
    int traffic_unit;
    int time_index; 
};

struct DiscreteEventTenant* createASiereofEmptyDiscreteEventTenant(long time_interval){
    struct DiscreteEventTenant* discreteEventTenants = (struct DiscreteEventTenant*)malloc(sizeof(struct DiscreteEventTenant)*time_interval);
    for(int index=0;index<time_interval;index++){
        struct DiscreteEventTenant discreteEventTenant;
        
        discreteEventTenant.traffic = 0;
        discreteEventTenant.traffic_unit = KBPS;
        discreteEventTenant.time_index = index;

        *(discreteEventTenants+index) = discreteEventTenant;
    }
    
    return discreteEventTenants;
}

#endif

#ifdef EVENT



#endif