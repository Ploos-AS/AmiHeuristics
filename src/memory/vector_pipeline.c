#include "amiheuristics/vector_pipeline.h"

#define AMIHEUR_PIPELINE_MAX_VECTORS 128U

static int build_specs(const AmiHeurVectorSlot *slots,
                       const char *const *names,
                       const unsigned long *expected_starts,
                       const unsigned long *expected_ends,
                       size_t vector_count,
                       AmiHeurVectorSpec *specs)
{
    size_t i;
    for (i = 0U; i < vector_count; ++i) {
        if (names[i] == NULL) return -1;
        if (slots[i].encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG)
            return -3;
        specs[i].name = names[i];
        specs[i].target = slots[i].target;
        specs[i].expected_start = expected_starts[i];
        specs[i].expected_end = expected_ends[i];
        specs[i].target_code = NULL;
        specs[i].target_code_size = 0U;
    }
    return 0;
}

static int validate(const AmiHeurVectorSlot *slots,
                    const char *const *names,
                    const unsigned long *expected_starts,
                    const unsigned long *expected_ends,
                    size_t vector_count,
                    const AmiHeurInventory *inventory,
                    size_t capacity,
                    size_t *required_count)
{
    if (required_count == NULL || inventory == NULL) return -1;
    *required_count = vector_count;
    if (vector_count > AMIHEUR_PIPELINE_MAX_VECTORS || vector_count > capacity)
        return -2;
    if (vector_count != 0U &&
        (slots == NULL || names == NULL || expected_starts == NULL ||
         expected_ends == NULL)) return -1;
    return 0;
}

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
    int rc = validate(slots, names, expected_starts, expected_ends,
                      vector_count, inventory, capacity, required_count);
    if (rc != 0) return rc;
    rc = build_specs(slots, names, expected_starts, expected_ends,
                     vector_count, specs);
    if (rc != 0) return rc;
    return amiheur_vector_inspect_with_allowlist(specs, vector_count,
                                                  inventory, rules, rule_count,
                                                  results, capacity,
                                                  required_count);
}

int amiheur_vector_pipeline_with_memory(const AmiHeurVectorSlot *slots,
                            const char *const *names,
                            const unsigned long *expected_starts,
                            const unsigned long *expected_ends,
                            size_t vector_count,
                            const AmiHeurInventory *inventory,
                            const AmiHeurPatchRule *rules,
                            size_t rule_count,
                            const AmiHeurCodeRegion *code_regions,
                            size_t code_region_count,
                            const unsigned char *memory,
                            unsigned long memory_start,
                            size_t memory_size,
                            AmiHeurVectorInspection *results,
                            size_t capacity,
                            size_t *required_count)
{
    AmiHeurVectorSpec specs[AMIHEUR_PIPELINE_MAX_VECTORS];
    AmiHeurTargetCode code[AMIHEUR_PIPELINE_MAX_VECTORS];
    size_t i;
    int rc = validate(slots, names, expected_starts, expected_ends,
                      vector_count, inventory, capacity, required_count);
    if (rc != 0) return rc;
    if (memory == NULL || memory_size == 0U) return -1;
    if (code_region_count != 0U && code_regions == NULL) return -1;
    rc = build_specs(slots, names, expected_starts, expected_ends,
                     vector_count, specs);
    if (rc != 0) return rc;

    for (i = 0U; i < vector_count; ++i) {
        rc = amiheur_target_code_read(code_regions, code_region_count,
                                      specs[i].target, memory, memory_start,
                                      memory_size,
                                      AMIHEUR_PATCH_FINGERPRINT_SIZE,
                                      &code[i]);
        if (rc < 0) return -4;
        if (rc == 1) {
            specs[i].target_code = code[i].bytes;
            specs[i].target_code_size = code[i].size;
        }
    }
    return amiheur_vector_inspect_with_allowlist(specs, vector_count,
                                                  inventory, rules, rule_count,
                                                  results, capacity,
                                                  required_count);
}

int amiheur_vector_pipeline_with_amiga_regions(const AmiHeurVectorSlot *slots,
                            const char *const *names,
                            const unsigned long *expected_starts,
                            const unsigned long *expected_ends,
                            size_t vector_count,
                            const AmiHeurInventory *inventory,
                            const AmiHeurPatchRule *rules,
                            size_t rule_count,
                            const AmiHeurAmigaCodeRegionSet *code_regions,
                            const unsigned char *memory,
                            unsigned long memory_start,
                            size_t memory_size,
                            AmiHeurVectorInspection *results,
                            size_t capacity,
                            size_t *required_count)
{
    if (code_regions == NULL) return -1;
    if (code_regions->count > AMIHEUR_AMIGA_CODE_REGION_MAX) return -1;
    return amiheur_vector_pipeline_with_memory(slots, names,
                            expected_starts, expected_ends, vector_count,
                            inventory, rules, rule_count,
                            code_regions->regions, code_regions->count,
                            memory, memory_start, memory_size,
                            results, capacity, required_count);
}
