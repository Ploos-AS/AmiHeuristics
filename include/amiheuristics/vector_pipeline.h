#ifndef AMIHEURISTICS_VECTOR_PIPELINE_H
#define AMIHEURISTICS_VECTOR_PIPELINE_H

#include <stddef.h>
#include "amiheuristics/vector_reader.h"
#include "amiheuristics/vector_inspect.h"
#include "amiheuristics/target_code.h"
#include "amiheuristics/amiga_code_regions.h"

int amiheur_vector_pipeline(const AmiHeurVectorSlot *slots,
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

/* Fingerprint bytes require two independent facts: allocation/owner
 * provenance from inventory, and explicit confirmed executable coverage from
 * code_regions. Library allocation bounds alone never authorize code reads. */
int amiheur_vector_pipeline_with_memory(const AmiHeurVectorSlot *slots,
                            const char *const *names,
                            const unsigned long *expected_starts,
                            const unsigned long *expected_ends,
                            size_t vector_count,
                            const AmiHeurInventory *inventory,
                            const AmiHeurPatchRule *rules,
                            size_t rule_count,
                            const AmiHeurCodeRegion *code_regions,
                            size_t code_region_count,
                            const unsigned char *memory,
                            unsigned long memory_start,
                            size_t memory_size,
                            AmiHeurVectorInspection *results,
                            size_t capacity,
                            size_t *required_count);

/* Convenience adapter for a validated Amiga code-region set. Resident
 * extents accepted by amiheur_amiga_code_regions_add_residents() can flow
 * directly into fingerprint authorization without weakening provenance. */
int amiheur_vector_pipeline_with_amiga_regions(const AmiHeurVectorSlot *slots,
                            const char *const *names,
                            const unsigned long *expected_starts,
                            const unsigned long *expected_ends,
                            size_t vector_count,
                            const AmiHeurInventory *inventory,
                            const AmiHeurPatchRule *rules,
                            size_t rule_count,
                            const AmiHeurAmigaCodeRegionSet *code_regions,
                            const unsigned char *memory,
                            unsigned long memory_start,
                            size_t memory_size,
                            AmiHeurVectorInspection *results,
                            size_t capacity,
                            size_t *required_count);

#endif
