/* inventory.c — in-core master table operations. All state is global;
 * storage.c owns moving it to/from disk. */
#include <string.h>
#include "inventory.h"

struct item g_items[MAX_ITEMS];
int g_item_count = 0;
int g_dirty = 0;

/* linear scan — fine for 5k records, was fine in 1996 too */
int inv_find(const char *sku)
{
    int i;
    for (i = 0; i < g_item_count; i++) {
        if (strcmp(g_items[i].sku, sku) == 0)
            return i;
    }
    return -1;
}

int inv_add(const struct item *it)
{
    if (g_item_count >= MAX_ITEMS)
        return -1;
    if (inv_find(it->sku) >= 0)
        return -2; /* duplicate SKU */
    g_items[g_item_count] = *it;
    g_items[g_item_count].active = 1;
    g_item_count++;
    g_dirty = 1;
    return g_item_count - 1;
}

int inv_update(int idx, const struct item *it)
{
    if (idx < 0 || idx >= g_item_count)
        return -1;
    /* SKU is the identity — never let an update change it */
    if (strcmp(g_items[idx].sku, it->sku) != 0)
        return -2;
    g_items[idx] = *it;
    g_dirty = 1;
    return 0;
}

int inv_deactivate(int idx)
{
    if (idx < 0 || idx >= g_item_count)
        return -1;
    g_items[idx].active = 0;
    g_dirty = 1;
    return 0;
}

long inv_total_value_cents(void)
{
    long total = 0;
    int i;
    for (i = 0; i < g_item_count; i++) {
        if (g_items[i].active)
            total += g_items[i].qty_on_hand * g_items[i].unit_cost_cents;
    }
    return total;
}
