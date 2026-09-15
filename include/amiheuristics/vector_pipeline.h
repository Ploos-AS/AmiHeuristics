#ifndef AMIHEURISTICS_VECTOR_PIPELINE_H
#define AMIHEURISTICS_VECTOR_PIPELINE_H

#include <stddef.h>
#include "amiheuristics/vector_reader.h"
#include "amiheuristics/vector_inspect.h"

/* Convert decoded classic vector slots into provenance inspection inputs.
 * names and expected ranges are supplied by the caller because their meaning
 * is library/device specific. Unknown vector encodings are rejected rather
 * than guessed.
 */
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

#endif
