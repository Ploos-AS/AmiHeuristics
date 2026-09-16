#ifndef AMIHEURISTICS_AMIGA_VECTOR_PIPELINE_H
#define AMIHEURISTICS_AMIGA_VECTOR_PIPELINE_H

#include <stddef.h>
#include "amiheuristics/inventory.h"
#include "amiheuristics/vector_inspect.h"

#ifdef AMIHEURISTICS_AMIGA_NATIVE
struct Library;

/* Native bridge: read classic six-byte vectors directly from a library or
 * device base and feed the decoded targets into the portable provenance
 * pipeline. The caller supplies semantic vector names and expected target
 * ranges; this function never invents those policy facts. Fingerprint reads
 * are deliberately excluded here until independently confirmed code regions
 * are available. */
int amiheur_amiga_vector_pipeline_native(
    const struct Library *base,
    const char *const *names,
    const unsigned long *expected_starts,
    const unsigned long *expected_ends,
    size_t vector_count,
    const AmiHeurInventory *inventory,
    const AmiHeurPatchRule *rules,
    size_t rule_count,
    AmiHeurVectorInspection *results,
    size_t capacity,
    size_t *required_count);
#endif

#endif
