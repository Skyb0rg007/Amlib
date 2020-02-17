#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-journal.h>

#define MESSAGE_ID "ee79c56851b34d7ab988991d3f6af135"

int main(void)
{
    errno = 0;
    sd_journal_send(
            "MESSAGE=Hello, there!",
            "MESSAGE_ID=" MESSAGE_ID,
            "PRIORITY=3",
            "HOME=%s", getenv("HOME"),
            "ERRNO=%d", errno,
            NULL);
    return 0;
}

