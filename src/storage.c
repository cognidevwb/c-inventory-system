/* storage.c — reads/writes the master table as raw structs. NOT portable
 * across compilers or architectures (padding, endianness) — every box in
 * the warehouse runs the same 32-bit build, so it never mattered. */
#include <stdio.h>
#include <string.h>
#include "inventory.h"
#include "storage.h"

int st_load(void)
{
    FILE *f = fopen(MASTER_FILE, "rb");
    size_t n;
    if (!f)
        return -1;
    n = fread(&g_item_count, sizeof g_item_count, 1, f);
    if (n != 1 || g_item_count < 0 || g_item_count > MAX_ITEMS) {
        fclose(f);
        g_item_count = 0;
        return -1;
    }
    n = fread(g_items, sizeof(struct item), (size_t)g_item_count, f);
    fclose(f);
    if ((int)n != g_item_count) {
        g_item_count = 0;
        return -1;
    }
    g_dirty = 0;
    return 0;
}

int st_save(void)
{
    FILE *f;
    /* keep one generation of backup — the only "transaction" we have */
    remove(BACKUP_FILE);
    rename(MASTER_FILE, BACKUP_FILE);

    f = fopen(MASTER_FILE, "wb");
    if (!f)
        return -1;
    if (fwrite(&g_item_count, sizeof g_item_count, 1, f) != 1) {
        fclose(f);
        return -1;
    }
    if (fwrite(g_items, sizeof(struct item), (size_t)g_item_count, f)
        != (size_t)g_item_count) {
        fclose(f);
        return -1;
    }
    fclose(f);
    g_dirty = 0;
    return 0;
}
