#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/libraries.h>

#include "amiheuristics/amiga_vector_pipeline.h"
#include "amiheuristics/vector_pipeline.h"
#include "amiheuristics/vector_reader.h"

int amiheur_amiga_vector_pipeline_native(
    const struct Library *base,
    const char *const *names,
    const unsigned long *expected_starts,
    const unsigned long *expected_ends,
    size_t vector_count,
    const AmiHeurInventory *inventory,
    const AmiHeurPatchRule *rules,
    size_t rule_count,
    AmiHeurVectorInspection *results,
    size_t capacity,
    size_t *required_count)
{
    size_t i;
    size_t one_required;
    AmiHeurVectorSlot slot;
    int rc;

    if (required_count == NULL || base == NULL || inventory == NULL)
        return -1;
    *required_count = vector_count;
    if (vector_count != 0U &&
        (names == NULL || expected_starts == NULL || expected_ends == NULL))
        return -1;
    if (capacity != 0U && results == NULL) return -1;
    if (vector_count > capacity) return -2;

    /* Validate policy inputs before touching result storage. */
    for (i = 0U; i < vector_count; ++i) {
        if (names[i] == NULL || expected_starts[i] > expected_ends[i])
            return -1;
    }

    for (i = 0U; i < vector_count; ++i) {
        rc = amiheur_vector_read_native(base, i + 1U, &slot, 1U,
                                        &one_required);
        if (rc == -2) {
            /* The reader's batch API cannot place the Nth slot in one cell.
             * Decode that slot directly from the bounded negative vector
             * area after proving it is within lib_NegSize. */
            size_t distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
            const unsigned char *base_bytes = (const unsigned char *)base;
            if (distance > (size_t)base->lib_NegSize) return -3;
            rc = amiheur_vector_decode_slot(base_bytes - distance,
                                            AMIHEUR_VECTOR_SLOT_SIZE,
                                            -(long)distance, &slot);
        }
        if (rc != 0) return -3;
        if (slot.encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG)
            return -3;

        one_required = 0U;
        rc = amiheur_vector_pipeline(&slot, &names[i], &expected_starts[i],
                                     &expected_ends[i], 1U, inventory,
                                     rules, rule_count, &results[i], 1U,
                                     &one_required);
        if (rc != 0 || one_required != 1U) return -4;
    }
    return 0;
}

#endif
