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

static void item_region(const AmiHeurInventoryItem *item,
                        AmiHeurMemoryRegion *region)
{
    region->start = item->start;
    region->end = item->end;
    region->owner_kind = owner_kind(item->kind);
    region->owner_name = item->name;
    region->executable = item->executable;
    region->allow_patch = item->allow_patch;
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
    for (i = 0U; i < n; ++i)
        item_region(&inventory->items[i], &regions[i]);
    return n;
}

int amiheur_inventory_find_region(const AmiHeurInventory *inventory,
                                  unsigned long address,
                                  AmiHeurMemoryRegion *region)
{
    size_t i;
    if (inventory == 0 || region == 0) return -1;
    for (i = 0U; i < inventory->count; ++i) {
        if (address < inventory->items[i].start ||
            address > inventory->items[i].end)
            continue;
        item_region(&inventory->items[i], region);
        return 1;
    }
    return 0;
}
