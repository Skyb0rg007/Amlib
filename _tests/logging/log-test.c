
#include <stdio.h>
#include "am/logging.h"

int main(void)
{
    const struct am_log_field fields[] = {
        { "MESSAGE", "Hello, there!", sizeof("Hello, there!") - 1 },
        { "AM_DOMAIN", "amlib", -1 }
    };
    am_log_structured_array(AM_LOG_LEVEL_NOTICE,   fields, AM_ARRAY_SIZE(fields));
    am_log_structured_array(AM_LOG_LEVEL_WARNING,  fields, AM_ARRAY_SIZE(fields));
    am_log_structured_array(AM_LOG_LEVEL_ERROR,    fields, AM_ARRAY_SIZE(fields));
    am_log_structured_array(AM_LOG_LEVEL_CRITICAL, fields, AM_ARRAY_SIZE(fields));
    am_log_structured_array(AM_LOG_LEVEL_CRITICAL, fields, 0);
    AM_DEBUG_HERE();
    am_error("Hello %d", 32);
    return 0;
}

