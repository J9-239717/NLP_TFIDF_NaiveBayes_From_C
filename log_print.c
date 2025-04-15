#include "log_print.h"

void log_message(enum LogLevel level,const char* fmt,...) {
    const char* level_str;
    switch (level) {
        case LOG_TEST:
            level_str = "           [TEST]";
            break;
        case LOG_FUNCTION_CALL:
            level_str = "[FUNCTION CALL]";
            break;
        case LOG_INFO:
            level_str = "[INFO]";
            break;
        case LOG_DEBUG:
            level_str = "           [DEBUG]";
            break;
        case LOG_ERROR:
            level_str = "           [ERROR]";
            break;
        case LOG_TIME:
            level_str = "       [TIME]";
            break;
        default:
            level_str = "[UNKNOWN]";
    }
    fprintf(stderr, "%s ", level_str);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr,fmt, args);
    va_end(args);
}