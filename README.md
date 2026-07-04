# Warehouse Inventory Manager (legacy C)

Menu-driven inventory control program, in continuous use since 1996.
Single-user, single-machine: every table lives in memory and persists as
raw structs on disk (one `.BAK` generation is the only recovery story).

    make && ./invmgr

Tables (fixed caps): items 5,000 (`INVMAST.DAT`), suppliers 500
(`SUPPLIER.DAT`), purchase orders 2,000 (`PORDERS.DAT`). Stock movements
append one line each to `AUDIT.LOG` (plain text — grep it). Site settings
come from `INVMGR.CFG` (company name on reports, report width, data dir).

Modules: `inventory.c` / `supplier.c` / `po.c` (in-core CRUD + PO
lifecycle), `storage.c` (binary load/save, all three tables),
`report.c` (line-printer reports), `audit.c` (movement log),
`config.c` (INVMGR.CFG), `main.c` (menu loop — the numbers 1–9 have not
changed since 1996 and never will; new functions get new numbers).

Known limitations the business has learned to live with: no concurrent
users, data files are not portable across architectures, the SKU field
silently truncates at 12 characters, a PO carries exactly one SKU
("phase 2", 1997, still pending), and the item `vendor` field is free
text that predates the supplier file — the two are not linked.
