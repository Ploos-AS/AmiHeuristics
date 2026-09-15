#include "amiheuristics/vector_reader.h"

int amiheur_vector_decode_slot(const unsigned char *slot,
                               size_t slot_size,
                               long offset,
                               AmiHeurVectorSlot *result)
{
    unsigned long target;

    if (slot == NULL || result == NULL) return -1;
    if (slot_size < AMIHEUR_VECTOR_SLOT_SIZE) return -1;
    if (offset >= 0L || ((-offset) % (long)AMIHEUR_VECTOR_SLOT_SIZE) != 0L)
        return -1;

    result->offset = offset;
    result->target = 0UL;
    result->encoding = AMIHEUR_VECTOR_ENCODING_UNKNOWN;

    /* Classic direct vector: JMP absolute long, opcode 4EF9 + 32-bit target. */
    if (slot[0] != 0x4eU || slot[1] != 0xf9U) return 0;

    target = ((unsigned long)slot[2] << 24) |
             ((unsigned long)slot[3] << 16) |
             ((unsigned long)slot[4] << 8) |
             (unsigned long)slot[5];
    result->target = target;
    result->encoding = AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG;
    return 1;
}

int amiheur_vector_decode_table(const unsigned char *table,
                                size_t table_size,
                                size_t slot_count,
                                AmiHeurVectorSlot *results,
                                size_t capacity,
                                size_t *required_count)
{
    size_t i;

    if (required_count == NULL) return -1;
    *required_count = slot_count;
    if (slot_count != 0U && table == NULL) return -1;
    if (capacity != 0U && results == NULL) return -1;
    if (slot_count > capacity) return -2;
    if (slot_count > table_size / AMIHEUR_VECTOR_SLOT_SIZE) return -1;

    for (i = 0U; i < slot_count; ++i) {
        long offset = -(long)((i + 1U) * AMIHEUR_VECTOR_SLOT_SIZE);
        int rc = amiheur_vector_decode_slot(table + i * AMIHEUR_VECTOR_SLOT_SIZE,
                                            AMIHEUR_VECTOR_SLOT_SIZE,
                                            offset, &results[i]);
        if (rc < 0) return -3;
    }
    return 0;
}
