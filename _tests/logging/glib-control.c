
#include <glib.h>
#include <errno.h>
#include <stdio.h>

int main(void)
{
    const GLogField fields[] = {
        { "MESSAGE", "Hello, there!", -1 },
        { "GLIB_DOMAIN", "amlib", -1 }
    };
    /* g_log_set_writer_func(g_log_writer_journald, NULL, NULL); */
    g_log_structured_array(G_LOG_LEVEL_MESSAGE, fields, G_N_ELEMENTS(fields));
    g_log_structured_array(G_LOG_LEVEL_WARNING, fields, G_N_ELEMENTS(fields));
    g_log_structured_array(G_LOG_LEVEL_CRITICAL, fields, G_N_ELEMENTS(fields));
    /* g_log_structured_array(G_LOG_LEVEL_ERROR, fields, G_N_ELEMENTS(fields)); */
    errno = EACCES;
    return 0;
}
