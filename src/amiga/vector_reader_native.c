#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/libraries.h>
#include "amiheuristics/vector_reader.h"

int amiheur_vector_read_native(const struct Library *base,
                               size_t slot_count,
                               AmiHeurVectorSlot *results,
                               size_t capacity,
                               size_t *required_count)
{
    size_t i;
    const unsigned char *base_bytes;

    if (required_count == NULL || base == NULL) return -1;
    *required_count = slot_count;
    if (capacity != 0U && results == NULL) return -1;
    if (slot_count > capacity) return -2;
    if (slot_count > (size_t)base->lib_NegSize / AMIHEUR_VECTOR_SLOT_SIZE)
        return -1;

    base_bytes = (const unsigned char *)base;
    for (i = 0U; i < slot_count; ++i) {
        size_t distance = (i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE;
        long offset = -(long)distance;
        int rc = amiheur_vector_decode_slot(base_bytes - distance,
                                            AMIHEUR_VECTOR_SLOT_SIZE,
                                            offset, &results[i]);
        if (rc < 0) return -3;
    }
    return 0;
}

#endif
