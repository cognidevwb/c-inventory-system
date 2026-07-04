/* supplier.h — supplier master record layout + CRUD. Same deal as the
 * item master: FIXED WIDTH, written raw to disk (SUPPLIER.DAT); change
 * this struct and every supplier file in the building is garbage. */
#ifndef SUPPLIER_H
#define SUPPLIER_H

#define SUPP_CODE_LEN    8
#define SUPP_NAME_LEN    30
#define SUPP_CONTACT_LEN 20
#define SUPP_PHONE_LEN   14
#define MAX_SUPPLIERS    500

struct supplier {
    char code[SUPP_CODE_LEN + 1];       /* short code, e.g. ACME01 */
    char name[SUPP_NAME_LEN + 1];
    char contact[SUPP_CONTACT_LEN + 1];
    char phone[SUPP_PHONE_LEN + 1];
    long lead_days;   /* typical lead time, used on the PO suggestions */
    int  active;      /* 0 = soft-deleted, record kept for audit */
};

/* the in-core supplier table, loaded once at startup */
extern struct supplier g_supps[MAX_SUPPLIERS];
extern int g_supp_count;
extern int g_supp_dirty; /* unsaved changes flag */

int sup_find(const char *code);
int sup_add(const struct supplier *sp);
int sup_update(int idx, const struct supplier *sp);
int sup_deactivate(int idx);

#endif
