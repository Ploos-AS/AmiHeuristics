#include "amiheuristics/amiga_snapshot.h"

int amiheur_amiga_snapshot_view(const AmiHeurAmigaSnapshot *snapshot,
                                AmiHeurSnapshotView *view)
{
    if (snapshot == NULL || view == NULL) return -1;
    if (snapshot->inventory.count > AMIHEUR_INVENTORY_MAX_ITEMS ||
        snapshot->vector_count > AMIHEUR_AMIGA_SNAPSHOT_MAX_VECTORS)
        return -1;

    view->items = snapshot->inventory.items;
    view->item_count = snapshot->inventory.count;
    view->vectors = snapshot->vectors;
    view->vector_count = snapshot->vector_count;
    return 0;
}
