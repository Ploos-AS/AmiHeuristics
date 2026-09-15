#ifndef AMIHEURISTICS_AMIGA_CODE_REGIONS_H
#define AMIHEURISTICS_AMIGA_CODE_REGIONS_H

#include <stddef.h>
#include "amiheuristics/target_code.h"

#define AMIHEUR_AMIGA_CODE_REGION_MAX 128U

typedef struct AmiHeurAmigaCodeRegionSet {
    AmiHeurCodeRegion regions[AMIHEUR_AMIGA_CODE_REGION_MAX];
    size_t count;
} AmiHeurAmigaCodeRegionSet;

void amiheur_amiga_code_regions_init(AmiHeurAmigaCodeRegionSet *set);

/* Add only ranges independently known to contain executable code.
 * Allocation ownership alone is not sufficient evidence.
 * Returns 0 success, -1 invalid input, -2 full.
 */
int amiheur_amiga_code_regions_add(AmiHeurAmigaCodeRegionSet *set,
                                   unsigned long start,
                                   unsigned long end);

#ifdef AMIHEURISTICS_AMIGA_NATIVE
/* Native baseline deliberately starts empty. The Exec library/device lists
 * expose allocation provenance but not trustworthy code extents. Native
 * adapters may add independently verified ROM/segment ranges later.
 */
int amiheur_amiga_code_regions_collect(AmiHeurAmigaCodeRegionSet *set);
#endif

#endif
