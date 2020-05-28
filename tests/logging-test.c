
#include <stdio.h>
#include "am/common.h"
#include "am/logging.h"

static am_log_writer_fn my_writer;

int main(void)
{
    am_log(AM_LOG_LEVEL_INFO, AM_LOG_STD_FLDS, "AMLIB_DOMAIN", "amlib-test", "MESSAGE", "This is my message: %d", 12);

    am_log_set_writer(my_writer, NULL);

    am_log(AM_LOG_LEVEL_INFO, AM_LOG_STD_FLDS, "AMLIB_DOMAIN", "amlib-test", "MESSAGE", "This is my message: %d", 12);
    return 0;
}

static int my_writer(
        enum am_log_level_flags log_level,
        const struct am_log_field *fields,
        size_t num_fields,
        void *ud)
{
    size_t i;
    (void)ud;
    (void)log_level;

    fprintf(stderr, "{\n");
    for (i = 0; i < num_fields; i++) {
        fprintf(stderr, "    \"%s\": \"%s\"%s\n",
                fields[i].key,
                (const char *)fields[i].value,
                i == num_fields - 1 ? "" : ",");
    }
    fprintf(stderr, "}\n");
    return AM_LOG_WRITER_HANDLED;
}

