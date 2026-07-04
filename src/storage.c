/* storage.c — reads/writes the master tables as raw structs. NOT portable
 * across compilers or architectures (padding, endianness) — every box in
 * the warehouse runs the same 32-bit build, so it never mattered.
 * File layout is always: int count, then count raw records. */
#include <stdio.h>
#include <string.h>
#include "inventory.h"
#include "supplier.h"
#include "po.h"
#include "storage.h"

static int st_read(const char *path, void *arr, size_t recsz, int cap,
                   int *count)
{
    FILE *f = fopen(path, "rb");
    size_t n;
    int cnt;
    if (!f)
        return -1;
    n = fread(&cnt, sizeof cnt, 1, f);
    if (n != 1 || cnt < 0 || cnt > cap) {
        fclose(f);
        *count = 0;
        return -1;
    }
    n = fread(arr, recsz, (size_t)cnt, f);
    fclose(f);
    if ((int)n != cnt) {
        *count = 0;
        return -1;
    }
    *count = cnt;
    return 0;
}

static int st_write(const char *path, const char *bak, const void *arr,
                    size_t recsz, int count)
{
    FILE *f;
    /* keep one generation of backup — the only "transaction" we have */
    remove(bak);
    rename(path, bak);

    f = fopen(path, "wb");
    if (!f)
        return -1;
    if (fwrite(&count, sizeof count, 1, f) != 1) {
        fclose(f);
        return -1;
    }
    if (fwrite(arr, recsz, (size_t)count, f) != (size_t)count) {
        fclose(f);
        return -1;
    }
    fclose(f);
    return 0;
}

int st_load(void)
{
    if (st_read(MASTER_FILE, g_items, sizeof(struct item), MAX_ITEMS,
                &g_item_count) != 0)
        return -1;
    g_dirty = 0;
    return 0;
}

int st_save(void)
{
    if (st_write(MASTER_FILE, BACKUP_FILE, g_items, sizeof(struct item),
                 g_item_count) != 0)
        return -1;
    g_dirty = 0;
    return 0;
}

int st_load_supp(void)
{
    if (st_read(SUPP_FILE, g_supps, sizeof(struct supplier), MAX_SUPPLIERS,
                &g_supp_count) != 0)
        return -1;
    g_supp_dirty = 0;
    return 0;
}

int st_save_supp(void)
{
    if (st_write(SUPP_FILE, SUPP_BAK, g_supps, sizeof(struct supplier),
                 g_supp_count) != 0)
        return -1;
    g_supp_dirty = 0;
    return 0;
}

int st_load_po(void)
{
    if (st_read(PO_FILE, g_pos, sizeof(struct po), MAX_POS,
                &g_po_count) != 0)
        return -1;
    g_po_dirty = 0;
    return 0;
}

int st_save_po(void)
{
    if (st_write(PO_FILE, PO_BAK, g_pos, sizeof(struct po),
                 g_po_count) != 0)
        return -1;
    g_po_dirty = 0;
    return 0;
}
