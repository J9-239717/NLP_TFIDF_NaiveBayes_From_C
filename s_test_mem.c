#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <time.h>

// Global stats
static long malloc_total_time = 0, calloc_total_time = 0, realloc_total_time = 0;
static size_t malloc_calls = 0, calloc_calls = 0, realloc_calls = 0;

void* malloc(size_t size) {
    static void* (*real_malloc)(size_t) = NULL;
    if (!real_malloc) real_malloc = dlsym(RTLD_NEXT, "malloc");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void* ptr = real_malloc(size);

    clock_gettime(CLOCK_MONOTONIC, &end);
    long ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    malloc_total_time += ns;
    malloc_calls++;

    fprintf(stderr, "[malloc] size=%zu ptr=%p time=%ld ns\n", size, ptr, ns);
    return ptr;
}

void* calloc(size_t nmemb, size_t size) {
    static void* (*real_calloc)(size_t, size_t) = NULL;
    if (!real_calloc) real_calloc = dlsym(RTLD_NEXT, "calloc");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void* ptr = real_calloc(nmemb, size);

    clock_gettime(CLOCK_MONOTONIC, &end);
    long ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    calloc_total_time += ns;
    calloc_calls++;

    fprintf(stderr, "[calloc] nmemb=%zu size=%zu total=%zu ptr=%p time=%ld ns\n",
            nmemb, size, nmemb * size, ptr, ns);
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    static void* (*real_realloc)(void*, size_t) = NULL;
    if (!real_realloc) real_realloc = dlsym(RTLD_NEXT, "realloc");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    void* new_ptr = real_realloc(ptr, size);

    clock_gettime(CLOCK_MONOTONIC, &end);
    long ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    realloc_total_time += ns;
    realloc_calls++;

    fprintf(stderr, "[realloc] old_ptr=%p size=%zu new_ptr=%p time=%ld ns\n",
            ptr, size, new_ptr, ns);
    return new_ptr;
}

void free(void* ptr) {
    static void (*real_free)(void*) = NULL;
    if (!real_free) real_free = dlsym(RTLD_NEXT, "free");
    real_free(ptr);
}

__attribute__((destructor))
static void report_stats() {
    fprintf(stderr, "\n==== malloc profiler summary ====\n");
    fprintf(stderr, "malloc: %zu calls, total: %ld ns, avg: %.2f ns\n",
            malloc_calls, malloc_total_time,
            malloc_calls ? (double)malloc_total_time / malloc_calls : 0);
    fprintf(stderr, "calloc: %zu calls, total: %ld ns, avg: %.2f ns\n",
            calloc_calls, calloc_total_time,
            calloc_calls ? (double)calloc_total_time / calloc_calls : 0);
    fprintf(stderr, "realloc: %zu calls, total: %ld ns, avg: %.2f ns\n",
            realloc_calls, realloc_total_time,
            realloc_calls ? (double)realloc_total_time / realloc_calls : 0);
    fprintf(stderr, "=================================\n");
}
