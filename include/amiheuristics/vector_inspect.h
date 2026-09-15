#ifndef AMIHEURISTICS_VECTOR_INSPECT_H
#define AMIHEURISTICS_VECTOR_INSPECT_H

#include <stddef.h>
#include "amiheuristics/inventory.h"
#include "amiheuristics/memory.h"
#include "amiheuristics/patch_allowlist.h"

typedef struct AmiHeurVectorSpec {
    const char *name;
    unsigned long target;
    unsigned long expected_start;
    unsigned long expected_end;
    const unsigned char *target_code;
    size_t target_code_size;
} AmiHeurVectorSpec;

typedef struct AmiHeurVectorInspection {
    const char *name;
    unsigned long target;
    int known_patch;
    size_t matched_patch_rule;
    int adjusted_score;
    AmiHeurMemoryReport provenance;
} AmiHeurVectorInspection;

/* Inspect caller-supplied vector targets against an inventory snapshot. */
int amiheur_vector_inspect(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count);

/* As above, with exact known-patch fingerprint recognition. */
int amiheur_vector_inspect_with_allowlist(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           const AmiHeurPatchRule *rules,
                           size_t rule_count,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count);

#endif
