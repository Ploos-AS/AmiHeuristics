#include "amiheuristics/m68k.h"

static unsigned int read_be16(const unsigned char *p)
{
    return ((unsigned int)p[0] << 8) | (unsigned int)p[1];
}

int amiheur_m68k_decode(const unsigned char *data, size_t size,
                        AmiHeurM68kInsn *insn)
{
    unsigned int op;

    if (data == 0 || insn == 0 || size < 2U)
        return -1;

    op = read_be16(data);
    insn->opcode = op;
    insn->length = 2U;
    insn->kind = AMIHEUR_M68K_OTHER;

    if ((op & 0xf000U) == 0x6000U) {
        insn->kind = AMIHEUR_M68K_BRANCH;
        insn->length = ((op & 0x00ffU) == 0U) ? 4U : 2U;
    } else if ((op & 0xffc0U) == 0x4e80U) {
        insn->kind = AMIHEUR_M68K_JSR;
    } else if ((op & 0xffc0U) == 0x4ec0U) {
        insn->kind = AMIHEUR_M68K_JMP;
    } else if (op == 0x4e75U) {
        insn->kind = AMIHEUR_M68K_RTS;
    } else if (op == 0x4e71U) {
        insn->kind = AMIHEUR_M68K_NOP;
    }

    if ((size_t)insn->length > size)
        return -1;

    return 0;
}
