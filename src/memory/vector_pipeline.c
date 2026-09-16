#include "amiheuristics/vector_pipeline.h"

#define AMIHEUR_PIPELINE_MAX_VECTORS 128U

static int build_spec(const AmiHeurVectorSlot *slot,
                      const char *name,
                      unsigned long expected_start,
                      unsigned long expected_end,
                      AmiHeurVectorSpec *spec)
{
    if (slot == NULL || name == NULL || spec == NULL) return -1;
    if (slot->encoding != AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG) return -3;
    spec->name = name;
    spec->target = slot->target;
    spec->expected_start = expected_start;
    spec->expected_end = expected_end;
    spec->target_code = NULL;
    spec->target_code_size = 0U;
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

static int inspect_one(const AmiHeurVectorSpec *spec,
                       const AmiHeurInventory *inventory,
                       const AmiHeurPatchRule *rules,
                       size_t rule_count,
                       AmiHeurVectorInspection *result)
{
    size_t one_required;
    return amiheur_vector_inspect_with_allowlist(spec, 1U, inventory,
                                                  rules, rule_count,
                                                  result, 1U,
                                                  &one_required);
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
    size_t i;
    int rc = validate(slots, names, expected_starts, expected_ends,
                      vector_count, inventory, capacity, required_count);
    if (rc != 0) return rc;
    if (vector_count != 0U && results == NULL) return -1;
    for (i = 0U; i < vector_count; ++i) {
        AmiHeurVectorSpec spec;
        rc = build_spec(&slots[i], names[i], expected_starts[i],
                        expected_ends[i], &spec);
        if (rc != 0) return rc;
        rc = inspect_one(&spec, inventory, rules, rule_count, &results[i]);
        if (rc != 0) return rc;
    }
    *required_count = vector_count;
    return 0;
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
    size_t i;
    int rc = validate(slots, names, expected_starts, expected_ends,
                      vector_count, inventory, capacity, required_count);
    if (rc != 0) return rc;
    if (memory == NULL || memory_size == 0U) return -1;
    if (code_region_count != 0U && code_regions == NULL) return -1;
    if (vector_count != 0U && results == NULL) return -1;

    for (i = 0U; i < vector_count; ++i) {
        AmiHeurVectorSpec spec;
        AmiHeurTargetCode code;
        rc = build_spec(&slots[i], names[i], expected_starts[i],
                        expected_ends[i], &spec);
        if (rc != 0) return rc;
        rc = amiheur_target_code_read(code_regions, code_region_count,
                                      spec.target, memory, memory_start,
                                      memory_size,
                                      AMIHEUR_PATCH_FINGERPRINT_SIZE,
                                      &code);
        if (rc < 0) return -4;
        if (rc == 1) {
            spec.target_code = code.bytes;
            spec.target_code_size = code.size;
        }
        rc = inspect_one(&spec, inventory, rules, rule_count, &results[i]);
        if (rc != 0) return rc;
    }
    *required_count = vector_count;
    return 0;
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
