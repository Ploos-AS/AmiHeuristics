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

int amiheur_amiga_code_regions_add_residents(
    AmiHeurAmigaCodeRegionSet *set,
    const AmiHeurResidentRegionView *views,
    size_t view_count)
{
    size_t i;
    int rc;

    if (set == NULL) return -1;
    if (view_count != 0U && views == NULL) return -1;

    for (i = 0U; i < view_count; ++i) {
        if (views[i].self_address == 0UL ||
            views[i].match_tag != views[i].self_address ||
            views[i].end_skip <= views[i].self_address)
            return -1;

        /* rt_EndSkip points immediately beyond the Resident module. */
        rc = amiheur_amiga_code_regions_add(set,
                                             views[i].self_address,
                                             views[i].end_skip - 1UL);
        if (rc != 0) return rc;
    }
    return 0;
}

#ifdef AMIHEURISTICS_AMIGA_NATIVE
int amiheur_amiga_code_regions_collect(AmiHeurAmigaCodeRegionSet *set)
{
    if (set == NULL) return -1;
    /* Fail closed: Exec's library/device allocation metadata does not prove
     * which bytes are executable code. Resident discovery is intentionally
     * separate because safely locating Resident structures requires an
     * independently bounded source rather than arbitrary pointer walking. */
    amiheur_amiga_code_regions_init(set);
    return 0;
}
#endif
