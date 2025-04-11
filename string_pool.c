#include "string_pool.h"

#define INITIAL_STRING_POOL_SIZE 1024*1024 // 1MB

StringPool* create_string_pool(){
    StringPool* pool = (StringPool*)malloc(sizeof(StringPool));
    checkExistMemory(pool);
    pool->capacity = INITIAL_STRING_POOL_SIZE;
    pool->buffer = (char*)malloc(pool->capacity);
    checkExistMemory(pool->buffer);
    pool->used = 0;
    return pool;
}

void destroy_string_pool(StringPool* pool){
    if(pool){
        free(pool->buffer);
        free(pool);
    }
}

char* str_pool_alloc(StringPool* pool,const char* str){
    int len = strlen(str) +1; // +1 for null terminator
    if(pool->used + len > pool->capacity){
        int new_capacity = pool->capacity * 2;
        while(pool->used + len >= new_capacity){
            new_capacity *= 2;
        }
        char* new_buffer = (char*)realloc(pool->buffer, new_capacity);
        printf("Reallocating memory for string pool\n");
        checkExistMemory(new_buffer);
        pool->buffer = new_buffer;
        pool->capacity = new_capacity;
    }
    char* ptr = pool->buffer + pool->used;
    strcpy(ptr, str);
    pool->used += len;
    return ptr;
}

void reset_string_pool(StringPool* pool){
    if(pool){
        pool->used = 0;
    }
}