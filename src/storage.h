/* storage.h — raw binary persistence of the master table. */
#ifndef STORAGE_H
#define STORAGE_H

#define MASTER_FILE "INVMAST.DAT"
#define BACKUP_FILE "INVMAST.BAK"

int st_load(void);  /* master file → g_items; 0 ok, -1 missing/corrupt */
int st_save(void);  /* g_items → master file (backs up the old one first) */

#endif
