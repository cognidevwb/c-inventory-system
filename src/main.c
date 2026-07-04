/* main.c — menu loop. Running since 1996; please do not "improve" the
 * menu numbers, the warehouse staff know them by muscle memory. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "inventory.h"
#include "storage.h"
#include "report.h"

static void read_line(char *buf, int cap)
{
    if (fgets(buf, cap, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

static void prompt_item(struct item *it)
{
    char buf[64];
    printf("SKU: ");
    read_line(it->sku, sizeof it->sku);
    printf("Description: ");
    read_line(it->desc, sizeof it->desc);
    printf("Vendor: ");
    read_line(it->vendor, sizeof it->vendor);
    printf("Qty on hand: ");
    read_line(buf, sizeof buf);
    it->qty_on_hand = atol(buf);
    printf("Reorder point: ");
    read_line(buf, sizeof buf);
    it->reorder_point = atol(buf);
    printf("Unit cost (cents): ");
    read_line(buf, sizeof buf);
    it->unit_cost_cents = atol(buf);
    it->active = 1;
}

static void do_receive(void)
{
    char sku[SKU_LEN + 1], buf[32];
    int idx;
    long qty;
    printf("SKU received: ");
    read_line(sku, sizeof sku);
    idx = inv_find(sku);
    if (idx < 0) {
        printf("unknown SKU\n");
        return;
    }
    printf("Qty received: ");
    read_line(buf, sizeof buf);
    qty = atol(buf);
    if (qty <= 0) {
        printf("bad qty\n");
        return;
    }
    g_items[idx].qty_on_hand += qty;
    g_dirty = 1;
    printf("ok — %s now %ld on hand\n", sku, g_items[idx].qty_on_hand);
}

static void do_issue(void)
{
    char sku[SKU_LEN + 1], buf[32];
    int idx;
    long qty;
    printf("SKU issued: ");
    read_line(sku, sizeof sku);
    idx = inv_find(sku);
    if (idx < 0) {
        printf("unknown SKU\n");
        return;
    }
    printf("Qty issued: ");
    read_line(buf, sizeof buf);
    qty = atol(buf);
    if (qty <= 0 || qty > g_items[idx].qty_on_hand) {
        printf("bad qty (have %ld)\n", g_items[idx].qty_on_hand);
        return;
    }
    g_items[idx].qty_on_hand -= qty;
    g_dirty = 1;
    if (g_items[idx].qty_on_hand <= g_items[idx].reorder_point)
        printf("*** %s is at/below reorder point ***\n", sku);
}

int main(void)
{
    char choice[8];

    if (st_load() != 0)
        printf("no master file — starting empty\n");

    for (;;) {
        printf("\n== WAREHOUSE INVENTORY ==\n"
               " 1 add item      4 receive stock   7 reorder list\n"
               " 2 edit item     5 issue stock     8 valuation\n"
               " 3 delete item   6 stock listing   9 save & quit\n> ");
        read_line(choice, sizeof choice);

        if (strcmp(choice, "1") == 0) {
            struct item it;
            memset(&it, 0, sizeof it);
            prompt_item(&it);
            if (inv_add(&it) < 0)
                printf("add failed (full or duplicate SKU)\n");
        } else if (strcmp(choice, "2") == 0) {
            char sku[SKU_LEN + 1];
            int idx;
            printf("SKU to edit: ");
            read_line(sku, sizeof sku);
            idx = inv_find(sku);
            if (idx < 0) {
                printf("unknown SKU\n");
            } else {
                struct item it = g_items[idx];
                prompt_item(&it);
                strcpy(it.sku, sku); /* identity is immutable */
                inv_update(idx, &it);
            }
        } else if (strcmp(choice, "3") == 0) {
            char sku[SKU_LEN + 1];
            printf("SKU to delete: ");
            read_line(sku, sizeof sku);
            if (inv_deactivate(inv_find(sku)) != 0)
                printf("unknown SKU\n");
        } else if (strcmp(choice, "4") == 0) {
            do_receive();
        } else if (strcmp(choice, "5") == 0) {
            do_issue();
        } else if (strcmp(choice, "6") == 0) {
            rpt_stock_listing();
        } else if (strcmp(choice, "7") == 0) {
            rpt_reorder_list();
        } else if (strcmp(choice, "8") == 0) {
            rpt_valuation();
        } else if (strcmp(choice, "9") == 0) {
            if (st_save() != 0) {
                printf("SAVE FAILED — not quitting\n");
                continue;
            }
            return 0;
        } else {
            printf("?\n");
        }
    }
}
