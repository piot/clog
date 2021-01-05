/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <clog/console.h>

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

static const int level_colors[] = {94, 36, 32, 36, 33, 31, 35};

void clog_console(enum clog_type type, const char* string)
{
	char buffer[32];
	struct timeval now;

	int error_code = gettimeofday(&now, 0);
	if (error_code < 0) {
		snprintf(buffer, 32, "unknown time");
	} else {
		time_t epoch_seconds = now.tv_sec;
		const struct tm* tm_now = gmtime(&epoch_seconds);
		char time_buffer[32];
		strftime(time_buffer, 32, "%Y-%m-%d %H:%M:%S", tm_now);

		int millisecond = now.tv_usec / 1000.0;

		snprintf(buffer, 32,"%s.%03d", time_buffer, millisecond);
	}

	fprintf(stderr, "\033[%dm%s %s: %s \033[0m\n", level_colors[type], buffer,
			clog_type_string[type], string);
}
