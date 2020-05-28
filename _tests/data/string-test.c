
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "am/macros.h"
#include "am/data/string.h"

static void debug_str(const struct am_string *str)
{
    printf("str = { .short = %s, .size = %lu, .cap = %lu, .data = \"%.*s\" }\n",
            am__string_is_short(str) ? "true" : "false",
            am_string_size(str), am_string_capacity(str),
            (int)am_string_size(str), am_string_data(str));
}

int main(void)
{
    struct am_string str;
    bool ok;
    ok = am_string_init(&str, "Skye Soss", 9);
    assert(ok);
    
    debug_str(&str);

    ok = am_string_reserve(&str, 200);
    assert(ok);
    debug_str(&str);

    am_string_free(&str);
    return 0;
}

