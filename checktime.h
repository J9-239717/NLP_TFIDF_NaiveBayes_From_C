#ifndef _TIMEUTIL_H_
#define _TIMEUTIL_H_

#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
#endif

// Function prototype
void start_timer();
double stop_timer();

#define show_time() do{\
    double time = stop_timer();\
    printf("Time: %f seconds\n", time);\
}while(0);\

#endif // _TIMEUTIL_H_
