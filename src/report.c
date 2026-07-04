/* report.c — fixed-width report printing, line-printer layout. Column
 * layouts assume 80 columns; REPORT_WIDTH in INVMGR.CFG only stretches
 * the ruled lines (the accountant wanted "wider lines" in 2001). */
#include <stdio.h>
#include "inventory.h"
#include "supplier.h"
#include "po.h"
#include "config.h"
#include "report.h"

static void money(long cents, char *buf)
{
    sprintf(buf, "%ld.%02ld", cents / 100, cents % 100);
}

static void rule(void)
{
    int i;
    for (i = 0; i < g_cfg.report_width; i++)
        putchar('=');
    putchar('\n');
}

/* every report carries the company name — the printouts get filed */
static void header(const char *title)
{
    printf("%s\n%s\n", g_cfg.company, title);
    rule();
}

void rpt_stock_listing(void)
{
    int i;
    header("STOCK LISTING");
    printf("SKU          DESCRIPTION                              QTY    UNIT\n");
    printf("------------ ---------------------------------------- ------ --------\n");
    for (i = 0; i < g_item_count; i++) {
        char m[32];
        if (!g_items[i].active)
            continue;
        money(g_items[i].unit_cost_cents, m);
        printf("%-12s %-40s %6ld %8s\n",
               g_items[i].sku, g_items[i].desc, g_items[i].qty_on_hand, m);
    }
}

void rpt_reorder_list(void)
{
    int i, hits = 0;
    header("REORDER LIST — qty at or below reorder point");
    for (i = 0; i < g_item_count; i++) {
        if (!g_items[i].active)
            continue;
        if (g_items[i].qty_on_hand <= g_items[i].reorder_point) {
            printf("%-12s %-40s have %ld, reorder at %ld (vendor: %s)\n",
                   g_items[i].sku, g_items[i].desc,
                   g_items[i].qty_on_hand, g_items[i].reorder_point,
                   g_items[i].vendor);
            hits++;
        }
    }
    if (!hits)
        printf("(nothing to reorder)\n");
}

void rpt_valuation(void)
{
    char m[32];
    int i, live = 0;
    for (i = 0; i < g_item_count; i++) {
        if (g_items[i].active)
            live++;
    }
    header("INVENTORY VALUATION");
    money(inv_total_value_cents(), m);
    printf("TOTAL INVENTORY VALUE: $%s across %d SKUs\n", m, live);
}

void rpt_supplier_listing(void)
{
    int i;
    header("SUPPLIER FILE");
    printf("CODE     NAME                           CONTACT              PHONE          LEAD\n");
    printf("-------- ------------------------------ -------------------- -------------- ----\n");
    for (i = 0; i < g_supp_count; i++) {
        if (!g_supps[i].active)
            continue;
        printf("%-8s %-30s %-20s %-14s %4ld\n",
               g_supps[i].code, g_supps[i].name, g_supps[i].contact,
               g_supps[i].phone, g_supps[i].lead_days);
    }
}

void rpt_open_pos(void)
{
    int i, hits = 0;
    header("OPEN PURCHASE ORDERS");
    printf("PO#      SUPP     SKU          ORDERED  RECVD     UNIT\n");
    printf("-------- -------- ------------ ------- ------ --------\n");
    for (i = 0; i < g_po_count; i++) {
        char m[32];
        if (g_pos[i].status != PO_OPEN)
            continue;
        money(g_pos[i].unit_cost_cents, m);
        printf("%8ld %-8s %-12s %7ld %6ld %8s\n",
               g_pos[i].po_no, g_pos[i].supp, g_pos[i].sku,
               g_pos[i].qty_ordered, g_pos[i].qty_received, m);
        hits++;
    }
    if (!hits)
        printf("(no open purchase orders)\n");
}

void rpt_po_suggest(void)
{
    int i, hits = 0;
    header("SUGGESTED PURCHASE ORDERS");
    for (i = 0; i < g_item_count; i++) {
        long want;
        if (!g_items[i].active)
            continue;
        if (g_items[i].qty_on_hand > g_items[i].reorder_point)
            continue;
        /* order back up to twice the reorder point — house rule since '98 */
        want = g_items[i].reorder_point * 2 - g_items[i].qty_on_hand;
        if (want < 1)
            want = 1;
        printf("%-12s %-40.40s order %6ld from %s\n",
               g_items[i].sku, g_items[i].desc, want, g_items[i].vendor);
        hits++;
    }
    if (!hits)
        printf("(stock levels ok — nothing to order)\n");
}
