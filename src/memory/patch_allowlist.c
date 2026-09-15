#include <string.h>
#include "amiheuristics/patch_allowlist.h"

int amiheur_patch_allowlist_match(const AmiHeurPatchRule *rules,
                                  size_t rule_count,
                                  const AmiHeurPatchCandidate *candidate,
                                  size_t *matched_rule)
{
    size_t i;

    if (matched_rule != NULL) *matched_rule = (size_t)-1;
    if (candidate == NULL || candidate->vector_name == NULL ||
        candidate->owner_name == NULL)
        return -1;
    if (rule_count != 0U && rules == NULL) return -1;

    for (i = 0U; i < rule_count; ++i) {
        const AmiHeurPatchRule *rule = &rules[i];
        if (rule->vector_name == NULL || rule->owner_name == NULL ||
            rule->fingerprint_size == 0U ||
            rule->fingerprint_size > AMIHEUR_PATCH_FINGERPRINT_SIZE)
            return -1;
        if (strcmp(rule->vector_name, candidate->vector_name) != 0) continue;
        if (rule->owner_kind != candidate->owner_kind) continue;
        if (strcmp(rule->owner_name, candidate->owner_name) != 0) continue;
        if (candidate->code == NULL || candidate->code_size < rule->fingerprint_size)
            continue;
        if (memcmp(rule->fingerprint, candidate->code,
                   rule->fingerprint_size) != 0)
            continue;
        if (matched_rule != NULL) *matched_rule = i;
        return 1;
    }
    return 0;
}
