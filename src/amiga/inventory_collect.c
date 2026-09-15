#include "amiheuristics/amiga_inventory.h"

int amiheur_amiga_inventory_collect_views(const AmiHeurAmigaNodeView *nodes,
                                          size_t node_count,
                                          AmiHeurInventory *inventory)
{
    size_t i;

    if (inventory == NULL) return -1;
    if (node_count != 0U && nodes == NULL) return -1;

    amiheur_inventory_init(inventory);
    for (i = 0U; i < node_count; ++i) {
        if (nodes[i].name == NULL || nodes[i].address == 0UL) return -1;
        if (amiheur_inventory_add(inventory, nodes[i].kind, nodes[i].name,
                                  nodes[i].address, nodes[i].address,
                                  1, 0) != 0)
            return -2;
    }
    return 0;
}
