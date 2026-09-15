#ifndef AMIHEURISTICS_AMIGA_INVENTORY_H
#define AMIHEURISTICS_AMIGA_INVENTORY_H

#include <stddef.h>
#include "amiheuristics/inventory.h"

typedef struct AmiHeurAmigaNodeView {
    AmiHeurInventoryKind kind;
    const char *name;
    unsigned long start;
    unsigned long end;
    int executable;
} AmiHeurAmigaNodeView;

/* Portable collector core. A view is an explicitly modelled range; callers
 * must not present a list-node address as an executable code extent. */
int amiheur_amiga_inventory_collect_views(const AmiHeurAmigaNodeView *nodes,
                                          size_t node_count,
                                          AmiHeurInventory *inventory);

#ifdef AMIHEURISTICS_AMIGA_NATIVE
/* Collect conservative native ranges on AmigaOS 2.04+.
 * Library/device allocations are represented using their negative/positive
 * sizes. Task list nodes are intentionally not claimed as executable code.
 */
int amiheur_amiga_inventory_collect(AmiHeurInventory *inventory);
#endif

#endif
