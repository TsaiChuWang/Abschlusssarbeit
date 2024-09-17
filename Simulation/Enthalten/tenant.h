
#define TENANT

#ifdef TENANT

struct Tenant{
    unsigned int identifier;

#ifdef DISCRETEEVENT
    // Discrete event
    struct DiscreteEventTenant *discreteEventTenants;

    int time_unit;
    long time_interval;
#endif

    struct Node connected_node;

};

struct Tenant createATenant(unsigned int* tenant_number, long time_interval){
    struct Tenant tenant;

    tenant.identifier = *tenant_number++;
    tenant.discreteEventTenants = createASiereofEmptyDiscreteEventTenant(time_interval);
    

    return tenant;
}

#endif

#ifdef DEBUG

void inform_discrete_event_atimeunit_foratenant(struct Tenant tenant, int time){
#ifdef KBPS
    printf("tenant["INFORM_IDENTIFIER_FORMAT"] input "INFORM_TRAFFIC_FORMAT" kbps in node["INFORM_IDENTIFIER_FORMAT"] at time["INFORM_TIME_FORMAT"].\n", tenant.identifier, tenant.discreteEventTenants[time].traffic, tenant.connected_node.identifier, time);
#endif
}

#endif