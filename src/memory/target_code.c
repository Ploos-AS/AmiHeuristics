#include <string.h>
#include "amiheuristics/target_code.h"

static int range_contains(unsigned long start, unsigned long end,
                          unsigned long target, size_t size)
{
    unsigned long last;
    if (size == 0U || start > end || target < start || target > end) return 0;
    if ((unsigned long)(size - 1U) > ~0UL - target) return 0;
    last = target + (unsigned long)(size - 1U);
    return last <= end;
}

int amiheur_target_code_read(const AmiHeurCodeRegion *code_regions,
                             size_t code_region_count,
                             unsigned long target,
                             const unsigned char *memory,
                             unsigned long memory_start,
                             size_t memory_size,
                             size_t requested_size,
                             AmiHeurTargetCode *result)
{
    size_t i;
    unsigned long memory_end;
    size_t offset;

    if (result == NULL) return -1;
    result->size = 0U;
    if (code_region_count != 0U && code_regions == NULL) return -1;
    if (requested_size == 0U || requested_size > AMIHEUR_TARGET_CODE_MAX)
        return -1;
    if (memory == NULL || memory_size == 0U) return -1;
    if ((unsigned long)(memory_size - 1U) > ~0UL - memory_start) return -1;
    memory_end = memory_start + (unsigned long)(memory_size - 1U);
    if (!range_contains(memory_start, memory_end, target, requested_size))
        return 0;

    for (i = 0U; i < code_region_count; ++i) {
        if (code_regions[i].start > code_regions[i].end) return -1;
        if (range_contains(code_regions[i].start, code_regions[i].end,
                           target, requested_size)) {
            offset = (size_t)(target - memory_start);
            memcpy(result->bytes, memory + offset, requested_size);
            result->size = requested_size;
            return 1;
        }
    }
    return 0;
}
