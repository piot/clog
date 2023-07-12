/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/console.h>

#include <stdio.h>
#include <time.h>

#if defined TORNADO_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdint.h>
int gettimeofday(struct timeval* tp, struct timezone* tzp)
{
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME systemTime;
    GetSystemTime(&systemTime);

    FILETIME fileTime;
    SystemTimeToFileTime(&systemTime, &fileTime);
    uint64_t time = ((uint64_t) fileTime.dwLowDateTime);
    time += ((uint64_t) fileTime.dwHighDateTime) << 32;

    tp->tv_sec = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (systemTime.wMilliseconds * 1000);

    return 0;
}
#define clog_gmtime_s gmtime_s
#else

#include <sys/time.h>

static struct tm* clog_gmtime_s(const time_t* restrict timer, struct tm* restrict buf)
{
    (void) buf;
    time_t temp = *timer;
    return gmtime((const time_t*) &temp);
}

#endif

static const int level_colors[] = {
    34, // VERBOSE
    36, // TRACE
    94, // DEBUG
    36, // INFO
    95, // NOTICE
    33, // WARN
    31, // ERROR
    35  // FATAL
};

void clog_console(enum clog_type type, const char* prefix, const char* string)
{
    char buffer[32];
    struct timeval now;

    int error_code = gettimeofday(&now, 0);
    if (error_code < 0) {
        snprintf(buffer, 32, "unknown time");
    } else {
        time_t epoch_seconds = now.tv_sec;
        struct tm tm_now;
        clog_gmtime_s((const time_t*) &epoch_seconds, &tm_now);
        char time_buffer[32];
        strftime(time_buffer, 32, "%Y-%m-%d %H:%M:%S", &tm_now);

        int millisecond = (int) (now.tv_usec / 1000);

        snprintf(buffer, 32, "%s.%03d", time_buffer, millisecond);
    }
    if (type > CLOG_TYPE_FATAL) {
        CLOG_BREAK;
    }

    fprintf(stderr, "\033[%dm%s %s: [%s] %s \033[0m\n", level_colors[type], buffer, clog_type_string[type], prefix,
            string);
    fflush(stderr);
}
