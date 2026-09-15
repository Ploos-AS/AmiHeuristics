#ifndef AMIHEURISTICS_PATCH_ALLOWLIST_H
#define AMIHEURISTICS_PATCH_ALLOWLIST_H

#include <stddef.h>
#include "amiheuristics/memory.h"

#define AMIHEUR_PATCH_FINGERPRINT_SIZE 8U

typedef struct AmiHeurPatchRule {
    const char *vector_name;
    AmiHeurMemoryOwnerKind owner_kind;
    const char *owner_name;
    unsigned char fingerprint[AMIHEUR_PATCH_FINGERPRINT_SIZE];
    size_t fingerprint_size;
} AmiHeurPatchRule;

typedef struct AmiHeurPatchCandidate {
    const char *vector_name;
    AmiHeurMemoryOwnerKind owner_kind;
    const char *owner_name;
    const unsigned char *code;
    size_t code_size;
} AmiHeurPatchCandidate;

/* Exact vector/owner/fingerprint match. Returns 1 match, 0 no match, -1 invalid. */
int amiheur_patch_allowlist_match(const AmiHeurPatchRule *rules,
                                  size_t rule_count,
                                  const AmiHeurPatchCandidate *candidate,
                                  size_t *matched_rule);

#endif
