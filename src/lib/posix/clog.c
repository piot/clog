#include <clog/clog.h>

char g_clog_temp_str[CLOG_TEMP_STR_SIZE];

void clogInitFromGlobal(Clog* self, const char* constantPrefix)
{
    self->config = &g_clog;
    self->constantPrefix = constantPrefix;
}
