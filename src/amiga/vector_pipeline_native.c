#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/libraries.h>

#include "amiheuristics/amiga_vector_pipeline.h"
#include "amiheuristics/vector_pipeline.h"
#include "amiheuristics/vector_reader.h"

#define AMIHEUR_NATIVE_VECTOR_MAX 128U

static int validate_native_vectors(const struct Library *base,
                                   const char *const *names,
                                   const unsigned long *expected_starts,
                                   const unsigned long *expected_ends,
                                   size_t vector_count)
{
    size_t i;
    size_t distance;
    const unsigned char *base_bytes;
    AmiHeurVectorSlot slot;
    int rc;

    if (vector_count > AMIHEUR_NATIVE_VECTOR_MAX) return -2;
    if (vector_count == 0U) return 0;
    if (names == NULL || expected_starts == NULL || expected_ends == NULL)
        return -1;

    base_bytes = (const unsigned char *)base;
    for (i = 0U; i < vector_count; ++i) {
        if (names[i] == NULL || expected_starts[i] > expected_ends[i])
            return -1;
        distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
        if (distance > (size_t)base->lib_NegSize) return -3;
        rc = amiheur_vector_decode_slot(base_bytes - distance,
                                        AMIHEUR_VECTOR_SLOT_SIZE,
                                        -(long)distance, &slot);
        if (rc != 0 || slot.encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG)
            return -3;
    }
    return 0;
}

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
    size_t distance;
    size_t one_required;
    const unsigned char *base_bytes;
    AmiHeurVectorSlot slot;
    int rc;

    if (required_count == NULL || base == NULL || inventory == NULL)
        return -1;
    *required_count = vector_count;
    if (vector_count > capacity) return -2;
    if (vector_count != 0U && results == NULL) return -1;

    /* Validate every slot before writing any result. This preserves
     * all-or-nothing output even when a later native vector is malformed or
     * uses an unsupported encoding. */
    rc = validate_native_vectors(base, names, expected_starts, expected_ends,
                                 vector_count);
    if (rc != 0) return rc;

    base_bytes = (const unsigned char *)base;
    for (i = 0U; i < vector_count; ++i) {
        distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
        rc = amiheur_vector_decode_slot(base_bytes - distance,
                                        AMIHEUR_VECTOR_SLOT_SIZE,
                                        -(long)distance, &slot);
        if (rc != 0) return -4;

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
