#define TOKEN_BUCKET_H

#define TOKEN_BUCKET_DATA_STORED_PATH   "../Datei/token_bucket/token_bucket.csv"

#ifdef TOKEN_BUCKET_H

struct Token_Bucket{
    double bucket_depth;
    double leakage_rate;

    double bucket_capacity;
};

struct Token_Bucket createATokenBucket(double bucket_depth, double leakage_rate){
    struct Token_Bucket bucket= {bucket_depth, leakage_rate, bucket_depth};
    return bucket;
}

struct Token_Bucket* createMultipleTokenBucket_SameParamter(double bucket_depth, double leakage_rate, int bucket_number){
    struct Token_Bucket* buckets = (struct Token_Bucket*)malloc(sizeof(struct Token_Bucket)*bucket_number);
    for(int index=0;index<bucket_number;index++)
        *(buckets+index) = createATokenBucket(bucket_depth, leakage_rate);
    return buckets;
}

double updateTokenBucketATimestamp(struct Token_Bucket* bucket, double traffic){

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
    printf("bucket.bucket_capacity : "INFORM_TRAFFIC_FORMAT" add :"INFORM_TRAFFIC_FORMAT " ", bucket->bucket_capacity, bucket->bucket_capacity-traffic);
#endif

    if(bucket->bucket_capacity-traffic < 0){
        bucket->bucket_capacity = bucket->bucket_capacity + bucket->leakage_rate;
        if(bucket->bucket_capacity >= bucket->bucket_depth)
            bucket->bucket_capacity = bucket->bucket_depth;

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Dropped : "INFORM_TRAFFIC_FORMAT"\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE *file_pointer;
        file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

        return 0.0;
    }

    if(bucket->bucket_capacity-traffic+bucket->leakage_rate < bucket->bucket_depth){
        bucket->bucket_capacity = bucket->bucket_capacity - traffic;
        bucket->bucket_capacity = bucket->bucket_capacity + bucket->leakage_rate;
        if(bucket->bucket_capacity >= bucket->bucket_depth)
            bucket->bucket_capacity = bucket->bucket_depth;
#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Over : "INFORM_TRAFFIC_FORMAT"\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE *file_pointer;
        file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

        return bucket->leakage_rate;
    }
    else{
        double leakage_traffic = bucket->bucket_capacity-traffic;
        bucket->bucket_capacity = bucket->bucket_depth;

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Less : "INFORM_TRAFFIC_FORMAT"\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE *file_pointer;
        file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

        return leakage_traffic;
    }

    return 0.0;
}

#endif