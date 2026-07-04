/* audit.h — append-only stock movement log (AUDIT.LOG, plain text). */
#ifndef AUDIT_H
#define AUDIT_H

#define AUDIT_FILE "AUDIT.LOG"

/* action is a short verb (RECV/ISSUE/PORECV/ADD/DELETE), qty is the
 * movement, balance is on-hand AFTER the movement */
void aud_log(const char *action, const char *sku, long qty, long balance);

#endif
