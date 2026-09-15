#ifndef AMIHEURISTICS_TARGET_CODE_H
#define AMIHEURISTICS_TARGET_CODE_H

#include <stddef.h>
#include "amiheuristics/inventory.h"

#define AMIHEUR_TARGET_CODE_MAX 8U

typedef struct AmiHeurTargetCode {
    unsigned char bytes[AMIHEUR_TARGET_CODE_MAX];
    size_t size;
} AmiHeurTargetCode;

/* Copy target bytes only when the complete requested range is contained in a
 * known executable inventory region. memory is a caller-supplied view whose
 * address range is memory_start..memory_start+memory_size-1; this portable
 * API never dereferences target as a host/native pointer.
 * Returns 1 copied, 0 target not safely covered, -1 invalid input.
 */
int amiheur_target_code_read(const AmiHeurInventory *inventory,
                             unsigned long target,
                             const unsigned char *memory,
                             unsigned long memory_start,
                             size_t memory_size,
                             size_t requested_size,
                             AmiHeurTargetCode *result);

#endif
