#define TEST
#define DEBUG


#ifdef TEST




#define TIME_INTERVAL 100
#define TIME_UNIT 0

#define DISCRETEEVENT

#define KBPS 1

#endif

#define RECORD

#if (defined SIMPLE_V1_00_918) || (defined SIMPLE_V2_925_GCRA)
    #define TEST_CONFIGURATION_LINK_CAPACITY    360 /** @brief Link capacity for test configuration **/
    #define TEST_CONFIGURATION_TIME_INTERVAL    100 /** @brief Time interval for the test configuration **/
    #define TEST_CONFIGURATION_TENANT_NUMBER    3   /** @brief Number of tenants in the test configuration **/

    #define PROBABILITY_COMPLY_MODULE           30  /** @brief Probability of complying module (in percentage). **/
    /** @def COMPLY_MODULE_OR_NORMALDISTRIBUTION
     *  @brief Decide between module or Gaussian distribution based on probability.
     *  @details 1 for module, 0 for Gaussian.
     */
    #define COMPLY_MODULE_OR_NORMALDISTRIBUTION (int)((rand()%100)+1)>PROBABILITY_COMPLY_MODULE?0:1 

    #define PROBABILITY_DROPPED                 10  /** Probability of traffic being dropped (in percentage). **/
    /** @def DECIDE_DROPPED_OR_NOT
     *  @brief Decide if the traffic is dropped based on probability.
     *  @details 1 for dropped, 0 for keep.
     */
    #define DECIDE_DROPPED_OR_NOT               (int)((rand()%100)+1)>PROBABILITY_DROPPED?0:1

    #define SCALE_GUARDED_TRAFFIC               (-1)*(1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05)) /** @brief Scale factor for guarded traffic. **/
    #define SCALE_MIDDLE_INTERVAL               (0.5 + ((double)rand()/RAND_MAX)*(0.95-0.5))        /** Scale factor for middle interval traffic. **/
    #define SCALE_DROPPED_TRAFFIC               (1.05 + ((double)rand()/RAND_MAX)*(1.25-1.05))      /** Scale factor for dropped traffic. **/

#ifdef SIMPLE_V2_925_GCRA
    #define BUCKET_DEPTH 200
    #define LEAKAGE_RATE 120

    #define _DEBUG_UPDATETOKENBUCKETATIMESTAMP
#endif
    
    // #define _DEBUG_PRINTTRAFFICATIMESTAMP
#endif