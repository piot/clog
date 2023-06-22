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
    CLOG_TYPE_NOTICE,
    CLOG_TYPE_WARN,
    CLOG_TYPE_ERROR,
    CLOG_TYPE_FATAL
};

static const char* clog_type_string[] = {"VERBOSE", "TRACE", "DEBUG", "INFO", "NOTICE", "WARN", "ERROR", "FATAL"};

typedef struct clog_config {
    void (*log)(enum clog_type type, const char* prefix, const char* string);
    enum clog_type level;
} clog_config;

typedef struct Clog {
    const char* constantPrefix;
    clog_config* config;
} Clog;

extern clog_config g_clog;

void clogInitFromGlobal(Clog* self, const char* constantPrefix);

#define CLOG_TEMP_STR_SIZE (128 * 1024)

#define CLOG_PLATFORM_SPRINTF_S(target, size, ...) tc_snprintf(target, size, __VA_ARGS__)

extern char g_clog_temp_str[];

#define CLOG_BREAK abort()

#define CLOG_C_EX(logtype, logger, ...)                                                                                \
    {                                                                                                                  \
        if (logtype >= g_clog.level) {                                                                                 \
            int _err = CLOG_PLATFORM_SPRINTF_S(g_clog_temp_str, CLOG_TEMP_STR_SIZE, __VA_ARGS__);                      \
            if (_err < 0) {                                                                                            \
                CLOG_BREAK;                                                                                            \
            }                                                                                                          \
            (logger)->config->log(logtype, (logger)->constantPrefix, g_clog_temp_str);                                 \
        }                                                                                                              \
    }

#define CLOG_EX(logtype, ...)                                                                                          \
    {                                                                                                                  \
        if (logtype >= g_clog.level) {                                                                                 \
            int _err = CLOG_PLATFORM_SPRINTF_S(g_clog_temp_str, CLOG_TEMP_STR_SIZE, __VA_ARGS__);                      \
            if (_err < 0) {                                                                                            \
                CLOG_BREAK;                                                                                            \
            }                                                                                                          \
            g_clog.log(logtype, "", g_clog_temp_str);                                                                  \
        }                                                                                                              \
    }

#if defined CONFIGURATION_DEBUG
#define CLOG_LOG_ENABLED 1
#define CLOG_VERBOSE(...) CLOG_EX(CLOG_TYPE_VERBOSE, __VA_ARGS__)
#define CLOG_EXECUTE(something) something
#define CLOG_INFO(...) CLOG_EX(CLOG_TYPE_INFO, __VA_ARGS__)
#define CLOG_DEBUG(...) CLOG_EX(CLOG_TYPE_DEBUG, __VA_ARGS__)
#define CLOG_WARN(...) CLOG_EX(CLOG_TYPE_WARN, __VA_ARGS__)
#define CLOG_SOFT_ERROR(...) CLOG_WARN(__VA_ARGS__);
#define CLOG_NOTICE(...) CLOG_EX(CLOG_TYPE_NOTICE, __VA_ARGS__)

#define CLOG_C_VERBOSE(logger, ...) CLOG_C_EX(CLOG_TYPE_VERBOSE, (logger), __VA_ARGS__)
#define CLOG_C_INFO(logger, ...) CLOG_C_EX(CLOG_TYPE_INFO, (logger), __VA_ARGS__)
#define CLOG_C_DEBUG(logger, ...) CLOG_C_EX(CLOG_TYPE_DEBUG, (logger), __VA_ARGS__)
#define CLOG_C_WARN(logger, ...) CLOG_C_EX(CLOG_TYPE_WARN, (logger), __VA_ARGS__)
#define CLOG_C_SOFT_ERROR(logger, ...) CLOG_C_EX(CLOG_TYPE_WARN, (logger), __VA_ARGS__)
#define CLOG_C_NOTICE(logger, ...) CLOG_C_EX(CLOG_TYPE_NOTICE, (logger), __VA_ARGS__)
#define CLOG_C_ERROR(logger, ...)                                                                                      \
    CLOG_C_EX(CLOG_TYPE_ERROR, (logger), __VA_ARGS__)                                                                  \
    CLOG_OUTPUT(__VA_ARGS__);                                                                                          \
    CLOG_BREAK;

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
#define CLOG_NOTICE(...)
#define CLOG_BREAK abort()
#define CLOG_ERROR(...)                                                                                                \
    CLOG_EX(CLOG_TYPE_ERROR, __VA_ARGS__);                                                                             \
    CLOG_BREAK;

#define CLOG_ASSERT(expression, ...)

#define CLOG_C_VERBOSE(logger, ...)
#define CLOG_C_VERBOSE(logger, ...)
#define CLOG_C_INFO(logger, ...)
#define CLOG_C_DEBUG(logger, ...)
#define CLOG_C_WARN(logger, ...)
#define CLOG_C_SOFT_ERROR(logger, ...)
#define CLOG_C_ERROR(logger, ...)                                                                                      \
    CLOG_C_EX(CLOG_TYPE_ERROR, (logger), __VA_ARGS__);                                                                 \
    CLOG_BREAK;

#define CLOG_C_NOTICE(logger, ...)

#define CLOG_EXECUTE(something)

#endif

#define CLOG_OUTPUT(...)                                                                                               \
    {                                                                                                                  \
        tc_fprintf(stdout, __VA_ARGS__);                                                                               \
        tc_fprintf(stdout, "\n");                                                                                      \
        tc_fflush(stdout);                                                                                             \
    }

#define CLOG_OUTPUT_STDERR(...)                                                                                        \
    {                                                                                                                  \
        tc_fprintf(stderr, __VA_ARGS__);                                                                               \
        tc_fprintf(stderr, "\n");                                                                                      \
        tc_fflush(stderr);                                                                                             \
    }

#endif
