/* config.c — reads INVMGR.CFG from the directory invmgr is started in.
 * Format is strict key=value, no spaces around the '='; '#' comments.
 * Unknown keys are ignored so old configs keep working forever. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

struct config g_cfg;

static void set_str(char *dst, int cap, const char *src)
{
    strncpy(dst, src, (size_t)cap);
    dst[cap] = '\0'; /* strncpy does not promise termination */
}

void cfg_load(void)
{
    FILE *f;
    char line[128];

    /* factory defaults, same as the day it was installed */
    set_str(g_cfg.company, COMPANY_LEN, "GENERAL WAREHOUSE CO");
    g_cfg.data_dir[0] = '\0';
    g_cfg.report_width = 80;

    f = fopen(CFG_FILE, "r");
    if (!f)
        return; /* no config file is not an error */

    while (fgets(line, sizeof line, f)) {
        char *eq, *val;
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '#' || line[0] == '\0')
            continue;
        eq = strchr(line, '=');
        if (!eq)
            continue; /* junk line — ops will notice on the report header */
        *eq = '\0';
        val = eq + 1;
        if (strcmp(line, "COMPANY") == 0) {
            set_str(g_cfg.company, COMPANY_LEN, val);
        } else if (strcmp(line, "DATA_DIR") == 0) {
            set_str(g_cfg.data_dir, DATADIR_LEN, val);
        } else if (strcmp(line, "REPORT_WIDTH") == 0) {
            int w = atoi(val);
            /* nobody owns a printer outside 40..132 columns */
            if (w >= 40 && w <= 132)
                g_cfg.report_width = w;
        }
    }
    fclose(f);
}
