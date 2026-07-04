/* po.h — purchase order records. One SKU per PO line, one line per PO;
 * multi-line orders were "phase 2" in 1997 and phase 2 never came.
 * Written raw to disk (PORDERS.DAT) like everything else here. */
#ifndef PO_H
#define PO_H

#include "inventory.h"
#include "supplier.h"

#define MAX_POS   2000

#define PO_OPEN   0
#define PO_CLOSED 1

struct po {
    long po_no;                       /* numbering starts at 1001 */
    char supp[SUPP_CODE_LEN + 1];
    char sku[SKU_LEN + 1];
    long qty_ordered;
    long qty_received;
    long unit_cost_cents;
    int  status;                      /* PO_OPEN / PO_CLOSED */
};

/* the in-core PO table, loaded once at startup */
extern struct po g_pos[MAX_POS];
extern int g_po_count;
extern int g_po_dirty; /* unsaved changes flag */

int  po_find(long po_no);
long po_create(const char *supp, const char *sku, long qty,
               long unit_cost_cents);
int  po_receive(long po_no, long qty);

#endif
