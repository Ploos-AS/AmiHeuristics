#include <stdio.h>
#include "amiheuristics/amiga_code_regions.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurAmigaCodeRegionSet set;
    AmiHeurResidentRegionView resident;
    size_t i;

    amiheur_amiga_code_regions_init(&set);
    CHECK(set.count == 0U);
    CHECK(amiheur_amiga_code_regions_add(&set, 0x1000UL, 0x1fffUL) == 0);
    CHECK(set.count == 1U);
    CHECK(set.regions[0].start == 0x1000UL);
    CHECK(set.regions[0].end == 0x1fffUL);
    CHECK(amiheur_amiga_code_regions_add(&set, 0UL, 0x100UL) == -1);
    CHECK(amiheur_amiga_code_regions_add(&set, 0x2000UL, 0x1000UL) == -1);

    amiheur_amiga_code_regions_init(&set);
    resident.self_address = 0x4000UL;
    resident.match_tag = 0x4000UL;
    resident.end_skip = 0x4800UL;
    CHECK(amiheur_amiga_code_regions_add_residents(&set, &resident, 1U) == 0);
    CHECK(set.count == 1U);
    CHECK(set.regions[0].start == 0x4000UL);
    CHECK(set.regions[0].end == 0x47ffUL);

    resident.match_tag = 0x4100UL;
    CHECK(amiheur_amiga_code_regions_add_residents(&set, &resident, 1U) == -1);
    resident.match_tag = resident.self_address;
    resident.end_skip = resident.self_address;
    CHECK(amiheur_amiga_code_regions_add_residents(&set, &resident, 1U) == -1);
    CHECK(amiheur_amiga_code_regions_add_residents(&set, NULL, 1U) == -1);

    amiheur_amiga_code_regions_init(&set);
    for (i = 0U; i < AMIHEUR_AMIGA_CODE_REGION_MAX; ++i)
        CHECK(amiheur_amiga_code_regions_add(&set,
              0x1000UL + (unsigned long)(i * 0x10U),
              0x100fUL + (unsigned long)(i * 0x10U)) == 0);
    CHECK(amiheur_amiga_code_regions_add(&set, 0x9000UL, 0x900fUL) == -2);

    printf("PASS: verified Resident code-region model\n");
    return 0;
}
