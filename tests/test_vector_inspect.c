#include <stdio.h>
#include <string.h>
#include "amiheuristics/vector_inspect.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurInventory inventory;
    AmiHeurVectorSpec vectors[4];
    AmiHeurVectorInspection results[4];
    AmiHeurPatchRule rule;
    unsigned char patch_code[8] = { 0x4e, 0xf9, 0x00, 0x02, 0x01, 0x00, 0x4e, 0x75 };
    unsigned char changed_code[8];
    size_t count;
    int rc;
    size_t i;

    amiheur_inventory_init(&inventory);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_LIBRARY,
          "dos.library", 0x10000UL, 0x10fffUL, 1, 0) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_RESIDENT,
          "known.patch", 0x20000UL, 0x20fffUL, 1, 0) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_TASK,
          "unknown.worker", 0x30000UL, 0x30fffUL, 1, 0) == 0);

    for (i = 0U; i < 4U; ++i) {
        vectors[i].target_code = NULL;
        vectors[i].target_code_size = 0U;
    }
    vectors[0].name = "dos.library/Open";
    vectors[0].target = 0x10100UL;
    vectors[0].expected_start = 0x10000UL;
    vectors[0].expected_end = 0x10fffUL;

    vectors[1].name = "dos.library/Close";
    vectors[1].target = 0x20100UL;
    vectors[1].expected_start = 0x10000UL;
    vectors[1].expected_end = 0x10fffUL;
    vectors[1].target_code = patch_code;
    vectors[1].target_code_size = sizeof(patch_code);

    vectors[2].name = "dos.library/LoadSeg";
    vectors[2].target = 0x30100UL;
    vectors[2].expected_start = 0x10000UL;
    vectors[2].expected_end = 0x10fffUL;

    vectors[3].name = "dos.library/Execute";
    vectors[3].target = 0x90000UL;
    vectors[3].expected_start = 0x10000UL;
    vectors[3].expected_end = 0x10fffUL;

    rc = amiheur_vector_inspect(vectors, 4U, &inventory, results, 4U, &count);
    CHECK(rc == 0);
    CHECK(count == 4U);
    CHECK(results[0].provenance.score == 0);
    CHECK(results[1].provenance.score == 20);
    CHECK(results[1].adjusted_score == 20);
    CHECK(results[1].known_patch == 0);
    CHECK(results[2].provenance.score == 35);
    CHECK(results[3].provenance.score == 45);

    memset(&rule, 0, sizeof(rule));
    rule.vector_name = "dos.library/Close";
    rule.owner_kind = AMIHEUR_MEMORY_OWNER_RESIDENT;
    rule.owner_name = "known.patch";
    memcpy(rule.fingerprint, patch_code, sizeof(patch_code));
    rule.fingerprint_size = sizeof(patch_code);

    rc = amiheur_vector_inspect_with_allowlist(vectors, 4U, &inventory,
                                                &rule, 1U, results, 4U, &count);
    CHECK(rc == 0);
    CHECK(results[1].known_patch == 1);
    CHECK(results[1].matched_patch_rule == 0U);
    CHECK(results[1].provenance.score == 20);
    CHECK(results[1].adjusted_score == 0);

    memcpy(changed_code, patch_code, sizeof(patch_code));
    changed_code[7] ^= 1U;
    vectors[1].target_code = changed_code;
    rc = amiheur_vector_inspect_with_allowlist(vectors, 4U, &inventory,
                                                &rule, 1U, results, 4U, &count);
    CHECK(rc == 0);
    CHECK(results[1].known_patch == 0);
    CHECK(results[1].adjusted_score == 20);

    rc = amiheur_vector_inspect(vectors, 4U, &inventory, results, 2U, &count);
    CHECK(rc == -2);
    CHECK(count == 4U);

    printf("PASS: vector provenance and patch allowlist scoring\n");
    return 0;
}
