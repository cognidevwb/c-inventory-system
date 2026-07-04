/* main.c — menu loop. Running since 1996; please do not "improve" the
 * menu numbers, the warehouse staff know them by muscle memory.
 * (New functions get NEW numbers appended at the bottom. Always.) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "inventory.h"
#include "supplier.h"
#include "po.h"
#include "storage.h"
#include "report.h"
#include "audit.h"
#include "config.h"

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
    aud_log("RECV", sku, qty, g_items[idx].qty_on_hand);
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
    aud_log("ISSUE", sku, qty, g_items[idx].qty_on_hand);
    if (g_items[idx].qty_on_hand <= g_items[idx].reorder_point)
        printf("*** %s is at/below reorder point ***\n", sku);
}

static void prompt_supplier(struct supplier *sp)
{
    char buf[32];
    printf("Code: ");
    read_line(sp->code, sizeof sp->code);
    printf("Name: ");
    read_line(sp->name, sizeof sp->name);
    printf("Contact: ");
    read_line(sp->contact, sizeof sp->contact);
    printf("Phone: ");
    read_line(sp->phone, sizeof sp->phone);
    printf("Lead days: ");
    read_line(buf, sizeof buf);
    sp->lead_days = atol(buf);
    sp->active = 1;
}

/* supplier maintenance is a sub-menu so the main menu numbers stay put */
static void do_suppliers(void)
{
    char choice[8];
    for (;;) {
        printf("\n-- SUPPLIER FILE --\n"
               " 1 list  2 add  3 edit  4 delete  5 back\n> ");
        read_line(choice, sizeof choice);
        if (strcmp(choice, "1") == 0) {
            rpt_supplier_listing();
        } else if (strcmp(choice, "2") == 0) {
            struct supplier sp;
            memset(&sp, 0, sizeof sp);
            prompt_supplier(&sp);
            if (sup_add(&sp) < 0)
                printf("add failed (full or duplicate code)\n");
        } else if (strcmp(choice, "3") == 0) {
            char code[SUPP_CODE_LEN + 1];
            int idx;
            printf("Code to edit: ");
            read_line(code, sizeof code);
            idx = sup_find(code);
            if (idx < 0) {
                printf("unknown code\n");
            } else {
                struct supplier sp = g_supps[idx];
                prompt_supplier(&sp);
                strcpy(sp.code, code); /* identity is immutable */
                sup_update(idx, &sp);
            }
        } else if (strcmp(choice, "4") == 0) {
            char code[SUPP_CODE_LEN + 1];
            printf("Code to delete: ");
            read_line(code, sizeof code);
            if (sup_deactivate(sup_find(code)) != 0)
                printf("unknown code\n");
        } else if (strcmp(choice, "5") == 0) {
            return;
        } else {
            printf("?\n");
        }
    }
}

static void do_raise_po(void)
{
    char supp[SUPP_CODE_LEN + 1], sku[SKU_LEN + 1], buf[32];
    long qty, cost, no;
    printf("Supplier code: ");
    read_line(supp, sizeof supp);
    printf("SKU: ");
    read_line(sku, sizeof sku);
    printf("Qty to order: ");
    read_line(buf, sizeof buf);
    qty = atol(buf);
    printf("Unit cost (cents): ");
    read_line(buf, sizeof buf);
    cost = atol(buf);
    no = po_create(supp, sku, qty, cost);
    if (no == -1)
        printf("PO file full\n");
    else if (no == -2)
        printf("unknown supplier code\n");
    else if (no == -3)
        printf("unknown SKU\n");
    else if (no == -4)
        printf("bad qty\n");
    else
        printf("raised PO %ld\n", no);
}

static void do_receive_po(void)
{
    char buf[32];
    long no, qty;
    int rc;
    printf("PO number: ");
    read_line(buf, sizeof buf);
    no = atol(buf);
    printf("Qty received: ");
    read_line(buf, sizeof buf);
    qty = atol(buf);
    rc = po_receive(no, qty);
    if (rc == -1)
        printf("no such PO\n");
    else if (rc == -2)
        printf("PO is not open\n");
    else if (rc == -3)
        printf("bad qty (check open balance on report 13)\n");
    else if (rc == -4)
        printf("SKU on this PO is gone from the master file\n");
    else
        printf("ok — PO %ld received\n", no);
}

int main(void)
{
    char choice[8];

    cfg_load();
    if (g_cfg.data_dir[0] != '\0' && chdir(g_cfg.data_dir) != 0)
        printf("cannot cd to %s — using current dir\n", g_cfg.data_dir);

    if (st_load() != 0)
        printf("no master file — starting empty\n");
    if (st_load_supp() != 0)
        printf("no supplier file — starting empty\n");
    if (st_load_po() != 0)
        printf("no PO file — starting empty\n");

    for (;;) {
        printf("\n== WAREHOUSE INVENTORY ==\n"
               " 1 add item      4 receive stock   7 reorder list\n"
               " 2 edit item     5 issue stock     8 valuation\n"
               " 3 delete item   6 stock listing   9 save & quit\n"
               "10 supplier file 11 raise PO       12 receive PO\n"
               "13 open POs      14 PO suggestions\n> ");
        read_line(choice, sizeof choice);

        if (strcmp(choice, "1") == 0) {
            struct item it;
            memset(&it, 0, sizeof it);
            prompt_item(&it);
            if (inv_add(&it) < 0)
                printf("add failed (full or duplicate SKU)\n");
            else
                aud_log("ADD", it.sku, it.qty_on_hand, it.qty_on_hand);
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
            int idx;
            printf("SKU to delete: ");
            read_line(sku, sizeof sku);
            idx = inv_find(sku);
            if (inv_deactivate(idx) != 0)
                printf("unknown SKU\n");
            else
                aud_log("DELETE", sku, 0L, g_items[idx].qty_on_hand);
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
            if (st_save() != 0 || st_save_supp() != 0 || st_save_po() != 0) {
                printf("SAVE FAILED — not quitting\n");
                continue;
            }
            return 0;
        } else if (strcmp(choice, "10") == 0) {
            do_suppliers();
        } else if (strcmp(choice, "11") == 0) {
            do_raise_po();
        } else if (strcmp(choice, "12") == 0) {
            do_receive_po();
        } else if (strcmp(choice, "13") == 0) {
            rpt_open_pos();
        } else if (strcmp(choice, "14") == 0) {
            rpt_po_suggest();
        } else {
            printf("?\n");
        }
    }
}
