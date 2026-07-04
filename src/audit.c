/* audit.c — every stock movement gets one line in AUDIT.LOG. Text on
 * purpose: when the binary .DAT files scare people, this is the file
 * they grep. Nothing ever deletes or rewrites it. */
#include <stdio.h>
#include <time.h>
#include "audit.h"

void aud_log(const char *action, const char *sku, long qty, long balance)
{
    FILE *f;
    char stamp[32];
    time_t now;

    f = fopen(AUDIT_FILE, "a");
    if (!f)
        return; /* never block a stock move on a logging failure */
    now = time(NULL);
    strftime(stamp, sizeof stamp, "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(f, "%s %-8s %-12s qty %6ld bal %6ld\n",
            stamp, action, sku, qty, balance);
    fclose(f);
}
