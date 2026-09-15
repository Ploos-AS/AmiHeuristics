#ifndef AMIHEURISTICS_VECTOR_READER_H
#define AMIHEURISTICS_VECTOR_READER_H

#include <stddef.h>

#define AMIHEUR_VECTOR_SLOT_SIZE 6U

typedef enum AmiHeurVectorEncoding {
    AMIHEUR_VECTOR_ENCODING_UNKNOWN = 0,
    AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG
} AmiHeurVectorEncoding;

typedef struct AmiHeurVectorSlot {
    long offset;
    unsigned long target;
    AmiHeurVectorEncoding encoding;
} AmiHeurVectorSlot;

/* Decode one classic 68k library/device vector slot from its six bytes. */
int amiheur_vector_decode_slot(const unsigned char *slot,
                               size_t slot_size,
                               long offset,
                               AmiHeurVectorSlot *result);

/* Decode consecutive six-byte slots ordered -6, -12, ... from a buffer. */
int amiheur_vector_decode_table(const unsigned char *table,
                                size_t table_size,
                                size_t slot_count,
                                AmiHeurVectorSlot *results,
                                size_t capacity,
                                size_t *required_count);

#ifdef AMIHEURISTICS_AMIGA_NATIVE
struct Library;
/* Read slots immediately before a classic AmigaOS library/device base. */
int amiheur_vector_read_native(const struct Library *base,
                               size_t slot_count,
                               AmiHeurVectorSlot *results,
                               size_t capacity,
                               size_t *required_count);
#endif

#endif
