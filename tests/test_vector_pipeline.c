#include <stdio.h>
#include <string.h>
#include "amiheuristics/vector_pipeline.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurInventory inventory;
    AmiHeurVectorSlot slots[3];
    AmiHeurVectorInspection results[3];
    AmiHeurPatchRule rule;
    AmiHeurCodeRegion code_region;
    unsigned char memory[0x2000];
    const char *names[3];
    unsigned long starts[3];
    unsigned long ends[3];
    size_t required;
    int rc;

    memset(memory, 0, sizeof(memory));
    amiheur_inventory_init(&inventory);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_LIBRARY,
          "dos.library", 0x10000UL, 0x10fffUL, 1, 0) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_RESIDENT,
          "patcher", 0x11000UL, 0x11fffUL, 1, 0) == 0);

    slots[0].offset = -6L; slots[0].target = 0x10100UL;
    slots[0].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    slots[1].offset = -12L; slots[1].target = 0x11100UL;
    slots[1].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    slots[2].offset = -18L; slots[2].target = 0x90000UL;
    slots[2].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    names[0] = "dos.library/Open"; names[1] = "dos.library/Close";
    names[2] = "dos.library/LoadSeg";
    starts[0] = starts[1] = starts[2] = 0x10000UL;
    ends[0] = ends[1] = ends[2] = 0x10fffUL;

    rc = amiheur_vector_pipeline(slots, names, starts, ends, 3U,
                                 &inventory, NULL, 0U, results, 3U, &required);
    CHECK(rc == 0 && results[1].provenance.score == 20);

    memcpy(memory + 0x1100, "PATCH123", 8U);
    rule.vector_name = names[1];
    rule.owner_kind = AMIHEUR_MEMORY_OWNER_RESIDENT;
    rule.owner_name = "patcher";
    memcpy(rule.fingerprint, "PATCH123", 8U); rule.fingerprint_size = 8U;

    /* Inventory ownership alone must not authorize reading target bytes. */
    rc = amiheur_vector_pipeline_with_memory(slots, names, starts, ends, 3U,
                                 &inventory, &rule, 1U, NULL, 0U,
                                 memory, 0x10000UL, sizeof(memory),
                                 results, 3U, &required);
    CHECK(rc == 0);
    CHECK(results[1].known_patch == 0);
    CHECK(results[1].adjusted_score == 20);

    code_region.start = 0x11100UL; code_region.end = 0x111ffUL;
    rc = amiheur_vector_pipeline_with_memory(slots, names, starts, ends, 3U,
                                 &inventory, &rule, 1U, &code_region, 1U,
                                 memory, 0x10000UL, sizeof(memory),
                                 results, 3U, &required);
    CHECK(rc == 0);
    CHECK(results[1].known_patch == 1);
    CHECK(results[1].adjusted_score == 0);
    CHECK(results[2].known_patch == 0);

    memory[0x1100] ^= 1U;
    rc = amiheur_vector_pipeline_with_memory(slots, names, starts, ends, 3U,
                                 &inventory, &rule, 1U, &code_region, 1U,
                                 memory, 0x10000UL, sizeof(memory),
                                 results, 3U, &required);
    CHECK(rc == 0 && results[1].known_patch == 0);

    slots[1].encoding = AMIHEUR_VECTOR_ENCODING_UNKNOWN;
    rc = amiheur_vector_pipeline(slots, names, starts, ends, 3U,
                                 &inventory, NULL, 0U, results, 3U, &required);
    CHECK(rc == -3);

    printf("PASS: confirmed-code vector fingerprint pipeline\n");
    return 0;
}
