#include <stdio.h>
#include <string.h>
#include "amiheuristics/vector_inspect.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurInventory inventory;
    AmiHeurVectorSpec vectors[4];
    AmiHeurVectorInspection results[4];
    size_t count;
    int rc;

    amiheur_inventory_init(&inventory);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_LIBRARY,
          "dos.library", 0x10000UL, 0x10fffUL, 1, 0) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_RESIDENT,
          "known.patch", 0x20000UL, 0x20fffUL, 1, 1) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_TASK,
          "unknown.worker", 0x30000UL, 0x30fffUL, 1, 0) == 0);

    vectors[0].name = "dos.library/Open";
    vectors[0].target = 0x10100UL;
    vectors[0].expected_start = 0x10000UL;
    vectors[0].expected_end = 0x10fffUL;

    vectors[1].name = "dos.library/Close";
    vectors[1].target = 0x20100UL;
    vectors[1].expected_start = 0x10000UL;
    vectors[1].expected_end = 0x10fffUL;

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
    CHECK(results[0].provenance.target_in_expected_range == 1);
    CHECK(results[1].provenance.score == 5);
    CHECK(results[1].provenance.target_allow_patch == 1);
    CHECK(results[2].provenance.score == 35);
    CHECK(results[2].provenance.target_owner_kind == AMIHEUR_MEMORY_OWNER_TASK);
    CHECK(results[3].provenance.score == 45);
    CHECK(results[3].provenance.target_in_known_region == 0);
    CHECK(strcmp(results[2].provenance.target_owner_name, "unknown.worker") == 0);

    rc = amiheur_vector_inspect(vectors, 4U, &inventory, results, 2U, &count);
    CHECK(rc == -2);
    CHECK(count == 4U);

    printf("PASS: vector provenance inspection\n");
    return 0;
}
