#include "h_files/checktime.h"

#ifdef _WIN32
    static LARGE_INTEGER start, end, frequency;
#else
    static struct timespec start, end;
#endif

void start_timer() {
#ifdef _WIN32
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
#else
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif
}

double stop_timer() {
#ifdef _WIN32
    QueryPerformanceCounter(&end);
    return (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart;
#else
    clock_gettime(CLOCK_MONOTONIC, &end);
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
#endif
}
