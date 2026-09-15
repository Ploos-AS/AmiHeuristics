#include "amiheuristics/vector_inspect.h"

static int inspect_vectors(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           const AmiHeurPatchRule *rules,
                           size_t rule_count,
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
    if (rule_count != 0U && rules == NULL) return -1;

    *required_count = vector_count;
    if (vector_count > capacity) return -2;

    region_count = amiheur_inventory_regions(inventory, regions,
                                             AMIHEUR_INVENTORY_MAX_ITEMS);
    if (region_count != inventory->count) return -3;

    for (i = 0U; i < vector_count; ++i) {
        AmiHeurVectorObservation obs;
        AmiHeurPatchCandidate candidate;
        int match;

        if (vectors[i].name == NULL ||
            vectors[i].expected_start > vectors[i].expected_end)
            return -1;
        obs.vector_name = vectors[i].name;
        obs.target = vectors[i].target;
        obs.expected_start = vectors[i].expected_start;
        obs.expected_end = vectors[i].expected_end;
        results[i].name = vectors[i].name;
        results[i].target = vectors[i].target;
        results[i].known_patch = 0;
        results[i].matched_patch_rule = (size_t)-1;
        if (amiheur_memory_analyze_vector(&obs, regions, region_count,
                                          &results[i].provenance) != 0)
            return -3;
        results[i].adjusted_score = results[i].provenance.score;

        if (rule_count == 0U || results[i].provenance.target_owner_name == NULL)
            continue;

        candidate.vector_name = vectors[i].name;
        candidate.owner_kind = results[i].provenance.target_owner_kind;
        candidate.owner_name = results[i].provenance.target_owner_name;
        candidate.code = vectors[i].target_code;
        candidate.code_size = vectors[i].target_code_size;
        match = amiheur_patch_allowlist_match(rules, rule_count, &candidate,
                                              &results[i].matched_patch_rule);
        if (match < 0) return -3;
        if (match == 1) {
            results[i].known_patch = 1;
            results[i].adjusted_score -= 20;
            if (results[i].adjusted_score < 0) results[i].adjusted_score = 0;
        }
    }
    return 0;
}

int amiheur_vector_inspect(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count)
{
    return inspect_vectors(vectors, vector_count, inventory, NULL, 0U,
                           results, capacity, required_count);
}

int amiheur_vector_inspect_with_allowlist(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           const AmiHeurPatchRule *rules,
                           size_t rule_count,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count)
{
    return inspect_vectors(vectors, vector_count, inventory, rules, rule_count,
                           results, capacity, required_count);
}
