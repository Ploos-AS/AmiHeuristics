#ifndef AMIHEURISTICS_INVENTORY_H
#define AMIHEURISTICS_INVENTORY_H

#include <stddef.h>
#include "amiheuristics/memory.h"

#define AMIHEUR_INVENTORY_MAX_ITEMS 128U

typedef enum AmiHeurInventoryKind {
    AMIHEUR_INVENTORY_RESIDENT = 0,
    AMIHEUR_INVENTORY_TASK,
    AMIHEUR_INVENTORY_LIBRARY,
    AMIHEUR_INVENTORY_DEVICE
} AmiHeurInventoryKind;

typedef struct AmiHeurInventoryItem {
    AmiHeurInventoryKind kind;
    const char *name;
    unsigned long start;
    unsigned long end;
    int executable;
    int allow_patch;
} AmiHeurInventoryItem;

typedef struct AmiHeurInventory {
    AmiHeurInventoryItem items[AMIHEUR_INVENTORY_MAX_ITEMS];
    size_t count;
} AmiHeurInventory;

void amiheur_inventory_init(AmiHeurInventory *inventory);
int amiheur_inventory_add(AmiHeurInventory *inventory,
                          AmiHeurInventoryKind kind,
                          const char *name,
                          unsigned long start,
                          unsigned long end,
                          int executable,
                          int allow_patch);
size_t amiheur_inventory_regions(const AmiHeurInventory *inventory,
                                 AmiHeurMemoryRegion *regions,
                                 size_t capacity);

/* Constant-space lookup used by low-memory live-vector inspection.
 * Returns 1 when address is owned, 0 when unknown, -1 on invalid input. */
int amiheur_inventory_find_region(const AmiHeurInventory *inventory,
                                  unsigned long address,
                                  AmiHeurMemoryRegion *region);

#endif
