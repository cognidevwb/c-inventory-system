/* storage.h — raw binary persistence of the master tables. Every table
 * gets one .DAT file and exactly one .BAK generation. */
#ifndef STORAGE_H
#define STORAGE_H

#define MASTER_FILE "INVMAST.DAT"
#define BACKUP_FILE "INVMAST.BAK"
#define SUPP_FILE   "SUPPLIER.DAT"
#define SUPP_BAK    "SUPPLIER.BAK"
#define PO_FILE     "PORDERS.DAT"
#define PO_BAK      "PORDERS.BAK"

int st_load(void);      /* master file → g_items; 0 ok, -1 missing/corrupt */
int st_save(void);      /* g_items → master file (backs up the old one first) */
int st_load_supp(void); /* SUPPLIER.DAT → g_supps */
int st_save_supp(void); /* g_supps → SUPPLIER.DAT */
int st_load_po(void);   /* PORDERS.DAT → g_pos */
int st_save_po(void);   /* g_pos → PORDERS.DAT */

#endif
