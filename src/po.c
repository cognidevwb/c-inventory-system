/* po.c — purchase order lifecycle: raise against a supplier + SKU,
 * receive against the open balance. Receiving is the ONLY code path
 * besides menu option 4 that touches qty_on_hand. */
#include <string.h>
#include "po.h"
#include "audit.h"

struct po g_pos[MAX_POS];
int g_po_count = 0;
int g_po_dirty = 0;

/* next number = highest on file + 1; scanning beats keeping a counter
 * file in sync (we tried that in '99, it drifted) */
static long next_po_no(void)
{
    long hi = 1000;
    int i;
    for (i = 0; i < g_po_count; i++) {
        if (g_pos[i].po_no > hi)
            hi = g_pos[i].po_no;
    }
    return hi + 1;
}

int po_find(long po_no)
{
    int i;
    for (i = 0; i < g_po_count; i++) {
        if (g_pos[i].po_no == po_no)
            return i;
    }
    return -1;
}

long po_create(const char *supp, const char *sku, long qty,
               long unit_cost_cents)
{
    struct po *p;
    long no;
    int si, ii;

    if (g_po_count >= MAX_POS)
        return -1;
    si = sup_find(supp);
    if (si < 0 || !g_supps[si].active)
        return -2;
    ii = inv_find(sku);
    if (ii < 0 || !g_items[ii].active)
        return -3;
    if (qty <= 0)
        return -4;

    no = next_po_no();
    p = &g_pos[g_po_count];
    memset(p, 0, sizeof *p);
    p->po_no = no;
    strcpy(p->supp, g_supps[si].code);
    strcpy(p->sku, g_items[ii].sku);
    p->qty_ordered = qty;
    p->qty_received = 0;
    p->unit_cost_cents = unit_cost_cents;
    p->status = PO_OPEN;
    g_po_count++;
    g_po_dirty = 1;
    return no;
}

int po_receive(long po_no, long qty)
{
    struct po *p;
    int pi, ii;

    pi = po_find(po_no);
    if (pi < 0)
        return -1;
    p = &g_pos[pi];
    if (p->status != PO_OPEN)
        return -2;
    if (qty <= 0 || qty > p->qty_ordered - p->qty_received)
        return -3;
    ii = inv_find(p->sku);
    if (ii < 0)
        return -4; /* SKU gone from master — hand-fix the files */

    p->qty_received += qty;
    if (p->qty_received >= p->qty_ordered)
        p->status = PO_CLOSED;
    g_items[ii].qty_on_hand += qty;
    g_dirty = 1;
    g_po_dirty = 1;
    aud_log("PORECV", p->sku, qty, g_items[ii].qty_on_hand);
    return 0;
}
