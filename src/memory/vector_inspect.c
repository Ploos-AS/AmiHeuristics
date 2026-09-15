#include "amiheuristics/vector_inspect.h"

int amiheur_vector_inspect(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count)
{
    AmiHeurMemoryRegion regions[AMIHEUR_INVENTORY_MAX_ITEMS];
    size_t region_count;
    size_t i;

    if (required_count == NULL || inventory == NULL) return -1;
    if (vector_count != 0U && vectors == NULL) return -1;
    if (capacity != 0U && results == NULL) return -1;

    *required_count = vector_count;
    if (vector_count > capacity) return -2;

    region_count = amiheur_inventory_regions(inventory, regions,
                                             AMIHEUR_INVENTORY_MAX_ITEMS);
    if (region_count != inventory->count) return -3;

    for (i = 0U; i < vector_count; ++i) {
        AmiHeurVectorObservation obs;
        if (vectors[i].name == NULL ||
            vectors[i].expected_start > vectors[i].expected_end)
            return -1;
        obs.vector_name = vectors[i].name;
        obs.target = vectors[i].target;
        obs.expected_start = vectors[i].expected_start;
        obs.expected_end = vectors[i].expected_end;
        results[i].name = vectors[i].name;
        results[i].target = vectors[i].target;
        if (amiheur_memory_analyze_vector(&obs, regions, region_count,
                                          &results[i].provenance) != 0)
            return -3;
    }
    return 0;
}
