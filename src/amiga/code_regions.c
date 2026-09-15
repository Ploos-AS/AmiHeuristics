#include "amiheuristics/amiga_code_regions.h"

void amiheur_amiga_code_regions_init(AmiHeurAmigaCodeRegionSet *set)
{
    if (set != NULL) set->count = 0U;
}

int amiheur_amiga_code_regions_add(AmiHeurAmigaCodeRegionSet *set,
                                   unsigned long start,
                                   unsigned long end)
{
    if (set == NULL || start == 0UL || start > end) return -1;
    if (set->count >= AMIHEUR_AMIGA_CODE_REGION_MAX) return -2;
    set->regions[set->count].start = start;
    set->regions[set->count].end = end;
    ++set->count;
    return 0;
}

#ifdef AMIHEURISTICS_AMIGA_NATIVE
int amiheur_amiga_code_regions_collect(AmiHeurAmigaCodeRegionSet *set)
{
    if (set == NULL) return -1;
    /* Fail closed: Exec's library/device allocation metadata does not prove
     * which bytes are executable code. Do not derive code regions from it. */
    amiheur_amiga_code_regions_init(set);
    return 0;
}
#endif
