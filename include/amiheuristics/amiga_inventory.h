#ifndef AMIHEURISTICS_AMIGA_INVENTORY_H
#define AMIHEURISTICS_AMIGA_INVENTORY_H

#include <stddef.h>
#include "amiheuristics/inventory.h"

typedef struct AmiHeurAmigaNodeView {
    AmiHeurInventoryKind kind;
    const char *name;
    unsigned long address;
} AmiHeurAmigaNodeView;

/*
 * Portable collector core used by the native AmigaOS adapter.
 * Nodes are copied into the caller-owned inventory without allocation.
 * Native code supplies stable names and validated addresses while the
 * relevant Exec list is protected.
 */
int amiheur_amiga_inventory_collect_views(const AmiHeurAmigaNodeView *nodes,
                                          size_t node_count,
                                          AmiHeurInventory *inventory);

#ifdef AMIHEURISTICS_AMIGA_NATIVE
/* Collect Exec residents/tasks/libraries/devices on AmigaOS 2.04+. */
int amiheur_amiga_inventory_collect(AmiHeurInventory *inventory);
#endif

#endif
