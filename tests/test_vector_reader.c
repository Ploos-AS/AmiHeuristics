#include <stdio.h>
#include "amiheuristics/vector_reader.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    unsigned char table[18] = {
        0x4e, 0xf9, 0x00, 0x01, 0x23, 0x40,
        0x4e, 0xf9, 0x00, 0xab, 0xcd, 0xef,
        0x4e, 0x75, 0x00, 0x00, 0x00, 0x00
    };
    AmiHeurVectorSlot slots[3];
    size_t required;
    int rc;

    rc = amiheur_vector_decode_table(table, sizeof(table), 3U,
                                     slots, 3U, &required);
    CHECK(rc == 0);
    CHECK(required == 3U);
    CHECK(slots[0].offset == -6L);
    CHECK(slots[0].encoding == AMIHEUR_VECTOR_ENCODING_JMP_ABS_LONG);
    CHECK(slots[0].target == 0x00012340UL);
    CHECK(slots[1].offset == -12L);
    CHECK(slots[1].target == 0x00abcdefUL);
    CHECK(slots[2].offset == -18L);
    CHECK(slots[2].encoding == AMIHEUR_VECTOR_ENCODING_UNKNOWN);
    CHECK(slots[2].target == 0UL);

    rc = amiheur_vector_decode_table(table, sizeof(table), 3U,
                                     slots, 2U, &required);
    CHECK(rc == -2);
    CHECK(required == 3U);

    CHECK(amiheur_vector_decode_slot(table, 5U, -6L, &slots[0]) == -1);
    CHECK(amiheur_vector_decode_slot(table, 6U, -7L, &slots[0]) == -1);
    CHECK(amiheur_vector_decode_table(table, 12U, 3U,
                                      slots, 3U, &required) == -1);

    printf("PASS: classic Amiga vector reader\n");
    return 0;
}
