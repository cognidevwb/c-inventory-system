/* inventory.h — item record layout + CRUD over the binary master file.
 * Record format is FIXED WIDTH and written raw to disk (INVMAST.DAT);
 * changing this struct breaks every existing data file. */
#ifndef INVENTORY_H
#define INVENTORY_H

#define SKU_LEN      12
#define DESC_LEN     40
#define VENDOR_LEN   20
#define MAX_ITEMS    5000

struct item {
    char  sku[SKU_LEN + 1];
    char  desc[DESC_LEN + 1];
    char  vendor[VENDOR_LEN + 1];
    long  qty_on_hand;
    long  reorder_point;
    /* stored in cents to avoid float money bugs (learned the hard way in '97) */
    long  unit_cost_cents;
    int   active; /* 0 = soft-deleted, record kept for audit */
};

/* the in-core master table, loaded once at startup */
extern struct item g_items[MAX_ITEMS];
extern int g_item_count;
extern int g_dirty; /* unsaved changes flag */

int  inv_find(const char *sku);
int  inv_add(const struct item *it);
int  inv_update(int idx, const struct item *it);
int  inv_deactivate(int idx);
long inv_total_value_cents(void);

#endif
