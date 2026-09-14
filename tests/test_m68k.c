#include "amiheuristics/m68k.h"

#include <stdio.h>

static int expect_kind(const unsigned char *bytes, size_t size,
                       AmiHeurM68kKind kind, unsigned int length)
{
    AmiHeurM68kInsn insn;

    if (amiheur_m68k_decode(bytes, size, &insn) != 0)
        return 1;
    if (insn.kind != kind || insn.length != length)
        return 1;
    return 0;
}

int main(void)
{
    static const unsigned char bra_short[] = { 0x60U, 0x02U };
    static const unsigned char bra_word[] = { 0x60U, 0x00U, 0x00U, 0x04U };
    static const unsigned char jsr_a0[] = { 0x4eU, 0x90U };
    static const unsigned char jmp_a0[] = { 0x4eU, 0xd0U };
    static const unsigned char rts[] = { 0x4eU, 0x75U };
    static const unsigned char nop[] = { 0x4eU, 0x71U };
    static const unsigned char other[] = { 0x12U, 0x34U };

    if (expect_kind(bra_short, sizeof(bra_short), AMIHEUR_M68K_BRANCH, 2U) != 0)
        return 1;
    if (expect_kind(bra_word, sizeof(bra_word), AMIHEUR_M68K_BRANCH, 4U) != 0)
        return 1;
    if (expect_kind(jsr_a0, sizeof(jsr_a0), AMIHEUR_M68K_JSR, 2U) != 0)
        return 1;
    if (expect_kind(jmp_a0, sizeof(jmp_a0), AMIHEUR_M68K_JMP, 2U) != 0)
        return 1;
    if (expect_kind(rts, sizeof(rts), AMIHEUR_M68K_RTS, 2U) != 0)
        return 1;
    if (expect_kind(nop, sizeof(nop), AMIHEUR_M68K_NOP, 2U) != 0)
        return 1;
    if (expect_kind(other, sizeof(other), AMIHEUR_M68K_OTHER, 2U) != 0)
        return 1;

    puts("PASS: minimal 68000 decoder");
    return 0;
}
