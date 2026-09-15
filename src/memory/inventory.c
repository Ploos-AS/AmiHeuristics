#include "amiheuristics/inventory.h"
#include <string.h>

static AmiHeurMemoryOwnerKind owner_kind(AmiHeurInventoryKind kind)
{
    switch (kind) {
    case AMIHEUR_INVENTORY_RESIDENT:
        return AMIHEUR_MEMORY_OWNER_RESIDENT;
    case AMIHEUR_INVENTORY_TASK:
        return AMIHEUR_MEMORY_OWNER_TASK;
    case AMIHEUR_INVENTORY_LIBRARY:
        return AMIHEUR_MEMORY_OWNER_LIBRARY;
    case AMIHEUR_INVENTORY_DEVICE:
        return AMIHEUR_MEMORY_OWNER_DEVICE;
    default:
        return AMIHEUR_MEMORY_OWNER_OTHER;
    }
}

void amiheur_inventory_init(AmiHeurInventory *inventory)
{
    if (inventory != 0)
        memset(inventory, 0, sizeof(*inventory));
}

int amiheur_inventory_add(AmiHeurInventory *inventory,
                          AmiHeurInventoryKind kind,
                          const char *name,
                          unsigned long start,
                          unsigned long end,
                          int executable,
                          int allow_patch)
{
    AmiHeurInventoryItem *item;

    if (inventory == 0 || name == 0 || start > end)
        return -1;
    if (inventory->count >= AMIHEUR_INVENTORY_MAX_ITEMS)
        return -1;

    item = &inventory->items[inventory->count++];
    item->kind = kind;
    item->name = name;
    item->start = start;
    item->end = end;
    item->executable = executable != 0;
    item->allow_patch = allow_patch != 0;
    return 0;
}

size_t amiheur_inventory_regions(const AmiHeurInventory *inventory,
                                 AmiHeurMemoryRegion *regions,
                                 size_t capacity)
{
    size_t i;
    size_t n;

    if (inventory == 0 || (regions == 0 && capacity != 0U))
        return 0U;

    n = inventory->count < capacity ? inventory->count : capacity;
    for (i = 0U; i < n; ++i) {
        regions[i].start = inventory->items[i].start;
        regions[i].end = inventory->items[i].end;
        regions[i].owner_kind = owner_kind(inventory->items[i].kind);
        regions[i].owner_name = inventory->items[i].name;
        regions[i].executable = inventory->items[i].executable;
        regions[i].allow_patch = inventory->items[i].allow_patch;
    }
    return n;
}
