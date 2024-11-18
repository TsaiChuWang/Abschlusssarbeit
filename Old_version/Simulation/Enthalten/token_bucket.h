/**
 * @file token_bucket.h
 * @brief Token bucket implementation for traffic shaping.
 *
 * This file contains functions for creating and updating token buckets,
 * which are used in network traffic shaping to control the flow of traffic.
 */

#ifndef TOKEN_BUCKET_H
#define TOKEN_BUCKET_H

// #define OVER_CAPACITY_DROP
// #define OVER_CAPACITY_LIMIT

#define TOKEN_BUCKET_DATA_STORED_PATH "../Datei/token_bucket/token_bucket.csv"

/**
 * @struct Token_Bucket
 * @brief Structure representing a token bucket.
 *
 * This structure holds information about the token bucket's depth, 
 * leakage rate, and current capacity.
 */
struct Token_Bucket {
    double bucket_depth;    /**< Maximum depth (capacity) of the bucket */
    double leakage_rate;    /**< Rate at which tokens are added to the bucket */
    double bucket_capacity; /**< Current amount of tokens in the bucket */
};

/**
 * @brief Creates a token bucket with the given parameters.
 *
 * Initializes a token bucket with the specified bucket depth and leakage rate.
 * The bucket is initially filled to its maximum capacity.
 *
 * @param bucket_depth The maximum depth of the token bucket.
 * @param leakage_rate The rate at which tokens are added to the bucket.
 * @return A newly created token bucket structure.
 */
struct Token_Bucket createATokenBucket(double bucket_depth, double leakage_rate) {
    struct Token_Bucket bucket = {bucket_depth, leakage_rate, bucket_depth};
    return bucket;
}

/**
 * @brief Creates multiple token buckets with the same parameters.
 *
 * Allocates and initializes multiple token buckets with the same bucket depth
 * and leakage rate.
 *
 * @param bucket_depth The maximum depth of the token buckets.
 * @param leakage_rate The rate at which tokens are added to the buckets.
 * @param bucket_number The number of token buckets to create.
 * @return A pointer to an array of initialized token buckets.
 */
struct Token_Bucket* createMultipleTokenBucket_SameParamter(double bucket_depth, double leakage_rate, int bucket_number) {
    struct Token_Bucket* buckets = (struct Token_Bucket*)malloc(sizeof(struct Token_Bucket) * bucket_number);
    for (int index = 0; index < bucket_number; index++) {
        *(buckets + index) = createATokenBucket(bucket_depth, leakage_rate);
    }
    return buckets;
}

/**
 * @brief Updates the token bucket at a specific timestamp.
 *
 * Simulates the removal of tokens from the bucket based on the traffic,
 * and replenishes tokens according to the leakage rate. If the bucket is 
 * underflowing, it handles the traffic drop accordingly.
 *
 * @param bucket Pointer to the token bucket to be updated.
 * @param traffic The amount of traffic to be processed by the bucket.
 * @return The amount of leakage traffic added to the bucket.
 */
double updateTokenBucketATimestamp(struct Token_Bucket* bucket, double traffic) {
#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
    printf("bucket.bucket_capacity : " INFORM_TRAFFIC_FORMAT " add :" INFORM_TRAFFIC_FORMAT " ",
           bucket->bucket_capacity, bucket->bucket_capacity - traffic);
#endif

    if (bucket->bucket_capacity - traffic < 0) {
#ifdef OVER_CAPACITY_DROP
        bucket->bucket_capacity = bucket->bucket_capacity + bucket->leakage_rate;
        if (bucket->bucket_capacity >= bucket->bucket_depth) {
            bucket->bucket_capacity = bucket->bucket_depth;
        }
#endif

#ifdef OVER_CAPACITY_LIMIT
        double leakage_traffic = bucket->bucket_capacity;
        bucket->bucket_capacity = bucket->leakage_rate;
        if (bucket->bucket_capacity >= bucket->bucket_depth) {
            bucket->bucket_capacity = bucket->bucket_depth;
        }
#endif

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Dropped : " INFORM_TRAFFIC_FORMAT "\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE* file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT ", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

#ifdef OVER_CAPACITY_DROP
        return 0.0;
#endif

#ifdef OVER_CAPACITY_LIMIT
    return leakage_traffic;
#endif
    }

    if (bucket->bucket_capacity - traffic + bucket->leakage_rate < bucket->bucket_depth) {
        bucket->bucket_capacity = bucket->bucket_capacity - traffic;
        bucket->bucket_capacity = bucket->bucket_capacity + bucket->leakage_rate;
        if (bucket->bucket_capacity >= bucket->bucket_depth) {
            bucket->bucket_capacity = bucket->bucket_depth;
        }

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Over : " INFORM_TRAFFIC_FORMAT "\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE* file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT ", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

        return bucket->leakage_rate;
    } else {
        double leakage_traffic = bucket->bucket_capacity - traffic;
        bucket->bucket_capacity = bucket->bucket_depth;

#ifdef _DEBUG_UPDATETOKENBUCKETATIMESTAMP
        printf("Less : " INFORM_TRAFFIC_FORMAT "\n", bucket->bucket_capacity);
#endif

#ifdef RECORD
        FILE* file_pointer = fopen(TOKEN_BUCKET_DATA_STORED_PATH, "a+");
        fprintf(file_pointer, INFORM_TRAFFIC_FORMAT ", ", bucket->bucket_capacity);
        fclose(file_pointer);
#endif

        return leakage_traffic;
    }

    return 0.0;
}

#endif // TOKEN_BUCKET_H
