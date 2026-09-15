#include <stdio.h>
#include <string.h>
#include "amiheuristics/patch_allowlist.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurPatchRule rule;
    AmiHeurPatchCandidate candidate;
    unsigned char good[8] = { 0x4e, 0xf9, 0x00, 0x02, 0x10, 0x00, 0x4e, 0x75 };
    unsigned char changed[8];
    size_t matched;

    memset(&rule, 0, sizeof(rule));
    rule.vector_name = "dos.library/Open";
    rule.owner_kind = AMIHEUR_MEMORY_OWNER_RESIDENT;
    rule.owner_name = "known.patch";
    memcpy(rule.fingerprint, good, sizeof(good));
    rule.fingerprint_size = sizeof(good);

    candidate.vector_name = "dos.library/Open";
    candidate.owner_kind = AMIHEUR_MEMORY_OWNER_RESIDENT;
    candidate.owner_name = "known.patch";
    candidate.code = good;
    candidate.code_size = sizeof(good);

    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == 1);
    CHECK(matched == 0U);

    candidate.vector_name = "dos.library/Close";
    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == 0);
    candidate.vector_name = "dos.library/Open";

    candidate.owner_name = "unknown.patch";
    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == 0);
    candidate.owner_name = "known.patch";

    memcpy(changed, good, sizeof(good));
    changed[6] ^= 1U;
    candidate.code = changed;
    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == 0);

    candidate.code = good;
    candidate.code_size = 4U;
    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == 0);

    rule.fingerprint_size = 0U;
    CHECK(amiheur_patch_allowlist_match(&rule, 1U, &candidate, &matched) == -1);

    printf("PASS: precise patch allowlist fingerprint matching\n");
    return 0;
}
