/* report.c — fixed-width report printing, 80-column printer layout. */
#include <stdio.h>
#include "inventory.h"
#include "report.h"

static void money(long cents, char *buf)
{
    sprintf(buf, "%ld.%02ld", cents / 100, cents % 100);
}

void rpt_stock_listing(void)
{
    int i;
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
    printf("*** REORDER LIST — qty at or below reorder point ***\n");
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
    money(inv_total_value_cents(), m);
    printf("TOTAL INVENTORY VALUE: $%s across %d SKUs\n", m, g_item_count);
}
