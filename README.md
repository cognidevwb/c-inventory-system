# Warehouse Inventory Manager (legacy C)

Menu-driven inventory control program, in continuous use since 1996.
Single-user, single-machine: the master table lives in memory (5,000-item
fixed cap) and persists as raw structs in `INVMAST.DAT` (one `.BAK`
generation is the only recovery story).

    make && ./invmgr

Modules: `inventory.c` (in-core CRUD), `storage.c` (binary load/save),
`report.c` (80-column printer reports), `main.c` (menu loop).

Known limitations the business has learned to live with: no concurrent
users, no audit trail beyond soft-delete, data file is not portable across
architectures, and the SKU field silently truncates at 12 characters.
