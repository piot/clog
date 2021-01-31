# clog

[![Actions Status](https://github.com/piot/clog/workflows/build%20linux/badge.svg)](https://github.com/piot/clog/actions)
[![Actions Status](https://github.com/piot/clog/workflows/build%20mac%20os/badge.svg)](https://github.com/piot/clog/actions)

Very simplistic logging for C.

## Description

Logging macros that log to stderr. It takes `sprintf` formatted arguments. Example:

```c
CLOG_INFO("Hello, %s", "world");
```

* `CLOG_VERBOSE()`. For logging that happens very frequently.
* `CLOG_INFO()`. Information that can be useful for the user.
* `CLOG_DEBUG()`. Internal debugging information. Rarely interesting for the user.
* `CLOG_WARN()`. Something that is not supposed to happen, but we can continue execution safely.
* `CLOG_SOFT_ERROR()`. Will be reported as an error, but execution doesn't stop.
* `CLOG_ERROR()`. Stops execution.
* `CLOG_OUTPUT()`. Outputs information no matter the log level. Will probably be deprecated.
* `CLOG_ASSERT(expression, ...)`. Checks that the expression is true, otherwise it issues a `CLOG_ERROR` with the format string.

## Usage
You need to provide a global `clog_config g_clog;` for it to link.

And in your `main()`, set a function to the `log` field that will handle the actual logging. Normally you use the provided [clog_console](https://github.com/piot/clog/blob/main/src/lib/posix/console.c).


```c
#include <clog/clog.h>
#include <clog/console.h> // optional, if you want to use the provided `clog_console`.

clog_config g_clog;

int main(int argc, const char* argv[])
{
    g_clog.log = clog_console;

    return 0;
}
```

