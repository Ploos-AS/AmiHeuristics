#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/libraries.h>

#include "amiheuristics/amiga_inventory.h"
#include "amiheuristics/amiga_snapshot.h"
#include "amiheuristics/vector_reader.h"

int amiheur_amiga_snapshot_capture(const struct Library *base,
                                   const char *const *vector_names,
                                   size_t vector_count,
                                   AmiHeurAmigaSnapshot *snapshot)
{
    size_t i;
    size_t distance;
    const unsigned char *base_bytes;
    AmiHeurVectorSlot slot;
    AmiHeurInventory inventory;
    int rc;

    if (base == NULL || snapshot == NULL) return -1;
    if (vector_count > AMIHEUR_AMIGA_SNAPSHOT_MAX_VECTORS) return -2;
    if (vector_count != 0U && vector_names == NULL) return -1;

    base_bytes = (const unsigned char *)base;

    /* Validate the entire requested vector set before changing snapshot. */
    for (i = 0U; i < vector_count; ++i) {
        if (vector_names[i] == NULL) return -1;
        distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
        if (distance > (size_t)base->lib_NegSize) return -3;
        rc = amiheur_vector_decode_slot(base_bytes - distance,
                                        AMIHEUR_VECTOR_SLOT_SIZE,
                                        -(long)distance, &slot);
        if (rc != 0 || slot.encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG)
            return -3;
    }

    rc = amiheur_amiga_inventory_collect(&inventory);
    if (rc != 0) return -4;

    /* Commit only after validation and inventory collection succeed. */
    snapshot->inventory = inventory;
    snapshot->vector_count = vector_count;
    for (i = 0U; i < vector_count; ++i) {
        distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
        rc = amiheur_vector_decode_slot(base_bytes - distance,
                                        AMIHEUR_VECTOR_SLOT_SIZE,
                                        -(long)distance, &slot);
        if (rc != 0) return -4;
        snapshot->vectors[i].name = vector_names[i];
        snapshot->vectors[i].target = slot.target;
    }
    return 0;
}

#endif
