#ifndef AMIHEURISTICS_M68K_H
#define AMIHEURISTICS_M68K_H

#include <stddef.h>

typedef enum AmiHeurM68kKind {
    AMIHEUR_M68K_OTHER = 0,
    AMIHEUR_M68K_BRANCH,
    AMIHEUR_M68K_JSR,
    AMIHEUR_M68K_JMP,
    AMIHEUR_M68K_RTS,
    AMIHEUR_M68K_NOP
} AmiHeurM68kKind;

typedef struct AmiHeurM68kInsn {
    AmiHeurM68kKind kind;
    unsigned int opcode;
    unsigned int length;
} AmiHeurM68kInsn;

int amiheur_m68k_decode(const unsigned char *data, size_t size,
                        AmiHeurM68kInsn *insn);

#endif
