/* supplier.c — in-core supplier table operations. Mirrors inventory.c
 * on purpose; if you learn one module you have learned them all. */
#include <string.h>
#include "supplier.h"

struct supplier g_supps[MAX_SUPPLIERS];
int g_supp_count = 0;
int g_supp_dirty = 0;

/* linear scan — we have ~60 live suppliers, relax */
int sup_find(const char *code)
{
    int i;
    for (i = 0; i < g_supp_count; i++) {
        if (strcmp(g_supps[i].code, code) == 0)
            return i;
    }
    return -1;
}

int sup_add(const struct supplier *sp)
{
    if (g_supp_count >= MAX_SUPPLIERS)
        return -1;
    if (sup_find(sp->code) >= 0)
        return -2; /* duplicate code */
    g_supps[g_supp_count] = *sp;
    g_supps[g_supp_count].active = 1;
    g_supp_count++;
    g_supp_dirty = 1;
    return g_supp_count - 1;
}

int sup_update(int idx, const struct supplier *sp)
{
    if (idx < 0 || idx >= g_supp_count)
        return -1;
    /* code is the identity — never let an update change it */
    if (strcmp(g_supps[idx].code, sp->code) != 0)
        return -2;
    g_supps[idx] = *sp;
    g_supp_dirty = 1;
    return 0;
}

int sup_deactivate(int idx)
{
    if (idx < 0 || idx >= g_supp_count)
        return -1;
    g_supps[idx].active = 0;
    g_supp_dirty = 1;
    return 0;
}
