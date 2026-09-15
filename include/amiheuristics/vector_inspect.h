#ifndef AMIHEURISTICS_VECTOR_INSPECT_H
#define AMIHEURISTICS_VECTOR_INSPECT_H

#include <stddef.h>
#include "amiheuristics/inventory.h"
#include "amiheuristics/memory.h"

typedef struct AmiHeurVectorSpec {
    const char *name;
    unsigned long target;
    unsigned long expected_start;
    unsigned long expected_end;
} AmiHeurVectorSpec;

typedef struct AmiHeurVectorInspection {
    const char *name;
    unsigned long target;
    AmiHeurMemoryReport provenance;
} AmiHeurVectorInspection;

/* Inspect caller-supplied vector targets against an inventory snapshot. */
int amiheur_vector_inspect(const AmiHeurVectorSpec *vectors,
                           size_t vector_count,
                           const AmiHeurInventory *inventory,
                           AmiHeurVectorInspection *results,
                           size_t capacity,
                           size_t *required_count);

#endif
