/* config.h — site settings from INVMGR.CFG (key=value, one per line).
 * Missing file or missing keys fall back to the 1996 factory defaults. */
#ifndef CONFIG_H
#define CONFIG_H

#define CFG_FILE     "INVMGR.CFG"
#define COMPANY_LEN  40
#define DATADIR_LEN  64

struct config {
    char company[COMPANY_LEN + 1];  /* printed at the top of every report */
    char data_dir[DATADIR_LEN + 1]; /* where the .DAT files live; "" = here */
    int  report_width;              /* printer carriage width, 40..132 */
};

extern struct config g_cfg;

void cfg_load(void);

#endif
