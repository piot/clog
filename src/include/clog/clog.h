/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef clog_h
#define clog_h

#include <tiny-libc/tiny_libc.h>

#include <stdio.h>
#include <stdlib.h>

enum clog_type {
    CLOG_TYPE_VERBOSE,
    CLOG_TYPE_TRACE,
    CLOG_TYPE_DEBUG,
    CLOG_TYPE_INFO,
    CLOG_TYPE_WARN,
    CLOG_TYPE_ERROR,
    CLOG_TYPE_FATAL
};

static const char* clog_type_string[] = {"VERBOSE", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

typedef struct clog_config {
    void (*log)(enum clog_type type, const char* string);
} clog_config;

extern clog_config g_clog;
extern char g_clog_temp_str[];                                                                                     \

#define CLOG_EX(logtype, ...)                                                                                          \
    {                                                                                                                  \
        sprintf(g_clog_temp_str, __VA_ARGS__);                                                                               \
        g_clog.log(logtype, g_clog_temp_str);                                                                                \
    }

#if defined CONFIGURATION_DEBUG
#define CLOG_LOG_ENABLED 1
#define CLOG_VERBOSE(...) CLOG_EX(CLOG_TYPE_VERBOSE, __VA_ARGS__)
#define CLOG_INFO(...) CLOG_EX(CLOG_TYPE_INFO, __VA_ARGS__)
#define CLOG_DEBUG(...) CLOG_EX(CLOG_TYPE_DEBUG, __VA_ARGS__)
#define CLOG_WARN(...) CLOG_EX(CLOG_TYPE_WARN, __VA_ARGS__)
#define CLOG_SOFT_ERROR(...) CLOG_WARN(__VA_ARGS__);
#define CLOG_BREAK abort()
#define CLOG_ERROR(...)                                                                                                \
    CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);                                                                             \
    CLOG_OUTPUT(__VA_ARGS__);                                                                                          \
    CLOG_BREAK;
#define CLOG_ASSERT(expression, ...)                                                                                   \
    if (!(expression)) {                                                                                               \
        CLOG_ERROR(__VA_ARGS__);                                                                                       \
        CLOG_BREAK;                                                                                                    \
    }

#else

#define CLOG_VERBOSE(...)
#define CLOG_INFO(...)
#define CLOG_DEBUG(...)
#define CLOG_WARN(...)
#define CLOG_SOFT_ERROR(...) CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);
#define CLOG_BREAK abort()
#define CLOG_ERROR(...)                                                                                                \
    CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);                                                                             \
    CLOG_BREAK;

#define CLOG_ASSERT(expression, ...)

#endif

#define CLOG_OUTPUT(...)                                                                                               \
    {                                                                                                                  \
        tc_fprintf(stdout, __VA_ARGS__);                                                                               \
        tc_fprintf(stdout, "\n");                                                                                      \
        tc_fflush(stdout);                                                                                             \
    }

#endif
