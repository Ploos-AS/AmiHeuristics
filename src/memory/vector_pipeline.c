#include "amiheuristics/vector_pipeline.h"

#define AMIHEUR_PIPELINE_MAX_VECTORS 128U

int amiheur_vector_pipeline(const AmiHeurVectorSlot *slots,
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
    AmiHeurVectorSpec specs[AMIHEUR_PIPELINE_MAX_VECTORS];
    size_t i;

    if (required_count == NULL || inventory == NULL) return -1;
    *required_count = vector_count;
    if (vector_count > AMIHEUR_PIPELINE_MAX_VECTORS) return -2;
    if (vector_count > capacity) return -2;
    if (vector_count != 0U &&
        (slots == NULL || names == NULL || expected_starts == NULL ||
         expected_ends == NULL)) return -1;

    for (i = 0U; i < vector_count; ++i) {
        if (names[i] == NULL) return -1;
        if (slots[i].encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG)
            return -3;
        specs[i].name = names[i];
        specs[i].target = slots[i].target;
        specs[i].expected_start = expected_starts[i];
        specs[i].expected_end = expected_ends[i];
        /* Native target bytes are deliberately not dereferenced here. */
        specs[i].target_code = NULL;
        specs[i].target_code_size = 0U;
    }

    return amiheur_vector_inspect_with_allowlist(specs, vector_count,
                                                  inventory, rules, rule_count,
                                                  results, capacity,
                                                  required_count);
}
