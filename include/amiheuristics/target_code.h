#ifndef AMIHEURISTICS_TARGET_CODE_H
#define AMIHEURISTICS_TARGET_CODE_H

#include <stddef.h>

#define AMIHEUR_TARGET_CODE_MAX 8U

typedef struct AmiHeurTargetCode {
    unsigned char bytes[AMIHEUR_TARGET_CODE_MAX];
    size_t size;
} AmiHeurTargetCode;

typedef struct AmiHeurCodeRegion {
    unsigned long start;
    unsigned long end;
} AmiHeurCodeRegion;

/* Copy fingerprint bytes only when the complete requested range is contained
 * in an explicitly confirmed code region AND the supplied memory view.
 * Allocation provenance (for example lib_NegSize/lib_PosSize) is deliberately
 * insufficient: callers must provide code_regions independently.
 * Returns 1 copied, 0 not safely covered, -1 invalid input.
 */
int amiheur_target_code_read(const AmiHeurCodeRegion *code_regions,
                             size_t code_region_count,
                             unsigned long target,
                             const unsigned char *memory,
                             unsigned long memory_start,
                             size_t memory_size,
                             size_t requested_size,
                             AmiHeurTargetCode *result);

#endif
