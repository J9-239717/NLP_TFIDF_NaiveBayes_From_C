#ifndef _IMPORT_H_
#define _IMPORT_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include <math.h>
#include <pthread.h>
#include "checktime.h"
#include "log_print.h"

#define DEBUG 0
#define ALPHA 1
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#define test(a,b) do{\
    if(a != b){\
        test_printf("[TEST FAILED]Test failed at line %d with different is %d and %d\n", __LINE__,a,b);\
    }else{\
        test_printf("[TEST PASS] Test in line %d pass in file %s\n", __LINE__,__FILE__);\
    }\
}while(0);\

#define sizeof_array(a) (sizeof(a)/sizeof(a[0]))

#define checkExistMemory(a) do{\
    if(a == NULL){\
        error_printf("Memory allocation failed at line %d in file %s\n", __LINE__, __FILE__);\
        exit(EXIT_FAILURE);\
    }\
}while(0);\

#endif