#ifndef AMIHEURISTICS_AMIGA_CODE_REGIONS_H
#define AMIHEURISTICS_AMIGA_CODE_REGIONS_H

#include <stddef.h>
#include "amiheuristics/target_code.h"

#define AMIHEUR_AMIGA_CODE_REGION_MAX 128U

typedef struct AmiHeurAmigaCodeRegionSet {
    AmiHeurCodeRegion regions[AMIHEUR_AMIGA_CODE_REGION_MAX];
    size_t count;
} AmiHeurAmigaCodeRegionSet;

typedef struct AmiHeurResidentRegionView {
    unsigned long match_tag;
    unsigned long self_address;
    unsigned long end_skip;
} AmiHeurResidentRegionView;

void amiheur_amiga_code_regions_init(AmiHeurAmigaCodeRegionSet *set);

/* Add only ranges independently known to contain executable code.
 * Allocation ownership alone is not sufficient evidence.
 * Returns 0 success, -1 invalid input, -2 full.
 */
int amiheur_amiga_code_regions_add(AmiHeurAmigaCodeRegionSet *set,
                                   unsigned long start,
                                   unsigned long end);

/* Convert validated Amiga Resident module extents into code-region evidence.
 * A Resident is accepted only when rt_MatchTag points to the Resident itself
 * and rt_EndSkip is strictly beyond it. This models the Resident module
 * extent, not arbitrary library/device allocation metadata.
 */
int amiheur_amiga_code_regions_add_residents(
    AmiHeurAmigaCodeRegionSet *set,
    const AmiHeurResidentRegionView *views,
    size_t view_count);

#ifdef AMIHEURISTICS_AMIGA_NATIVE
/* Native baseline remains fail-closed for library/device allocations.
 * Independently validated Resident extents may be supplied through the
 * portable add_residents core; native discovery is kept separate so unsafe
 * pointer walking is never required for ordinary provenance collection.
 */
int amiheur_amiga_code_regions_collect(AmiHeurAmigaCodeRegionSet *set);
#endif

#endif
