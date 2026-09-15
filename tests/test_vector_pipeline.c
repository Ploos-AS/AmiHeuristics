#include <stdio.h>
#include "amiheuristics/vector_pipeline.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurInventory inventory;
    AmiHeurVectorSlot slots[3];
    AmiHeurVectorInspection results[3];
    const char *names[3];
    unsigned long starts[3];
    unsigned long ends[3];
    size_t required;
    int rc;

    amiheur_inventory_init(&inventory);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_LIBRARY,
          "dos.library", 0x10000UL, 0x10fffUL, 1, 0) == 0);
    CHECK(amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_RESIDENT,
          "patcher", 0x20000UL, 0x20fffUL, 1, 0) == 0);

    slots[0].offset = -6L;
    slots[0].target = 0x10100UL;
    slots[0].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    slots[1].offset = -12L;
    slots[1].target = 0x20100UL;
    slots[1].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    slots[2].offset = -18L;
    slots[2].target = 0x90000UL;
    slots[2].encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;

    names[0] = "dos.library/Open";
    names[1] = "dos.library/Close";
    names[2] = "dos.library/LoadSeg";
    starts[0] = starts[1] = starts[2] = 0x10000UL;
    ends[0] = ends[1] = ends[2] = 0x10fffUL;

    rc = amiheur_vector_pipeline(slots, names, starts, ends, 3U,
                                 &inventory, NULL, 0U,
                                 results, 3U, &required);
    CHECK(rc == 0);
    CHECK(required == 3U);
    CHECK(results[0].target == 0x10100UL);
    CHECK(results[0].provenance.score == 0);
    CHECK(results[1].target == 0x20100UL);
    CHECK(results[1].provenance.score == 20);
    CHECK(results[2].target == 0x90000UL);
    CHECK(results[2].provenance.score == 45);

    slots[1].encoding = AMIHEUR_VECTOR_ENCODING_UNKNOWN;
    rc = amiheur_vector_pipeline(slots, names, starts, ends, 3U,
                                 &inventory, NULL, 0U,
                                 results, 3U, &required);
    CHECK(rc == -3);

    printf("PASS: decoded vector provenance pipeline\n");
    return 0;
}
