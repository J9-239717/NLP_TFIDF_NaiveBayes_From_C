#ifndef __LOG_PRINT_H__
#define __LOG_PRINT_H__
#include <stdio.h>
#include <stdarg.h>

enum LogLevel {
    LOG_TEST,
    LOG_FUNCTION_CALL,
    LOG_INFO,
    LOG_DEBUG,
    LOG_ERROR,
    LOG_TIME
};

void log_message(enum LogLevel level,const char* fmt,...);

#define test_printf(fmt, ...)          log_message(LOG_TEST, fmt, ##__VA_ARGS__)
#define func_printf(fmt, ...)          log_message(LOG_FUNCTION_CALL, fmt, ##__VA_ARGS__)
#define info_printf(fmt, ...)          log_message(LOG_INFO, fmt, ##__VA_ARGS__)
#define debug_printf(fmt, ...)         log_message(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define error_printf(fmt, ...)         log_message(LOG_ERROR, fmt, ##__VA_ARGS__)
#define time_printf(fmt, ...)         log_message(LOG_TIME, fmt, ##__VA_ARGS__)

#endif