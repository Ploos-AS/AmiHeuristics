#ifndef AMIHEURISTICS_VECTOR_PIPELINE_H
#define AMIHEURISTICS_VECTOR_PIPELINE_H

#include <stddef.h>
#include "amiheuristics/vector_reader.h"
#include "amiheuristics/vector_inspect.h"
#include "amiheuristics/target_code.h"

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

/* As above, but obtains fingerprint bytes from a bounded caller-supplied
 * memory view. Bytes are used only when target_code_read proves the complete
 * fingerprint lies in a known executable inventory region and the memory
 * view. Unsafe/uncovered targets remain analyzable but cannot be allowlisted.
 */
int amiheur_vector_pipeline_with_memory(const AmiHeurVectorSlot *slots,
                            const char *const *names,
                            const unsigned long *expected_starts,
                            const unsigned long *expected_ends,
                            size_t vector_count,
                            const AmiHeurInventory *inventory,
                            const AmiHeurPatchRule *rules,
                            size_t rule_count,
                            const unsigned char *memory,
                            unsigned long memory_start,
                            size_t memory_size,
                            AmiHeurVectorInspection *results,
                            size_t capacity,
                            size_t *required_count);

#endif
