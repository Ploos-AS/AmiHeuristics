#include "amiheuristics/bootblock.h"
#include "amiheuristics/exec_lvo.h"
#include "amiheuristics/m68k.h"
#include <limits.h>
#include <string.h>

#define AMIHEUR_U32_MASK 0xffffffffUL
#define AMIHEUR_BOOT_CODE_OFFSET 12U

static unsigned long read_be32(const unsigned char *p)
{
    return (((unsigned long)p[0] << 24) |
            ((unsigned long)p[1] << 16) |
            ((unsigned long)p[2] << 8) |
            (unsigned long)p[3]) & AMIHEUR_U32_MASK;
}

static unsigned long add_u32_carry(unsigned long sum, unsigned long value)
{
#if ULONG_MAX > 0xffffffffUL
    unsigned long total;

    total = (sum & AMIHEUR_U32_MASK) + (value & AMIHEUR_U32_MASK);
    sum = total & AMIHEUR_U32_MASK;
    if (total > AMIHEUR_U32_MASK)
        sum = (sum + 1UL) & AMIHEUR_U32_MASK;
    return sum;
#else
    unsigned long old_sum;

    old_sum = sum;
    sum += value;
    if (sum < old_sum)
        ++sum;
    return sum;
#endif
}

static unsigned long compute_checksum(const unsigned char *data)
{
    unsigned long sum;
    unsigned long value;
    unsigned int i;

    sum = 0UL;
    for (i = 0U; i < 256U; ++i) {
        value = (i == 1U) ? 0UL : read_be32(data + (i * 4U));
        sum = add_u32_carry(sum, value);
    }
    return (~sum) & AMIHEUR_U32_MASK;
}

static void add_finding(AmiHeurBootReport *report,
                        const char *rule_id,
                        int weight)
{
    if (report->finding_count < AMIHEUR_BOOT_MAX_FINDINGS)
        report->finding_ids[report->finding_count] = rule_id;
    ++report->finding_count;
    report->score += weight;
}

static int code_present(const unsigned char *data)
{
    unsigned int i;
    unsigned int nonzero;

    nonzero = 0U;
    for (i = AMIHEUR_BOOT_CODE_OFFSET; i < AMIHEUR_BOOTBLOCK_SIZE; ++i) {
        if (data[i] != 0U && data[i] != 0xffU) {
            ++nonzero;
            if (nonzero >= 8U)
                return 1;
        }
    }
    return 0;
}

static int has_decoded_kind(const unsigned char *data, AmiHeurM68kKind kind)
{
    unsigned int i;
    AmiHeurM68kInsn insn;

    for (i = AMIHEUR_BOOT_CODE_OFFSET;
         i + 1U < AMIHEUR_BOOTBLOCK_SIZE;
         i += 2U) {
        if (amiheur_m68k_decode(data + i,
                               AMIHEUR_BOOTBLOCK_SIZE - i,
                               &insn) == 0 &&
            insn.kind == kind)
            return 1;
    }
    return 0;
}

static int has_a6_lvo_call(const unsigned char *data)
{
    unsigned int i;
    AmiHeurM68kInsn insn;

    for (i = AMIHEUR_BOOT_CODE_OFFSET;
         i + 1U < AMIHEUR_BOOTBLOCK_SIZE;
         i += 2U) {
        if (amiheur_m68k_decode(data + i,
                               AMIHEUR_BOOTBLOCK_SIZE - i,
                               &insn) == 0 &&
            insn.is_a6_lvo &&
            insn.lvo_offset < 0)
            return 1;
    }
    return 0;
}

static int has_exec_lvo_class(const unsigned char *data,
                              AmiHeurExecLvoClass classification)
{
    unsigned int i;
    AmiHeurM68kInsn insn;
    AmiHeurExecLvoInfo info;

    for (i = AMIHEUR_BOOT_CODE_OFFSET;
         i + 1U < AMIHEUR_BOOTBLOCK_SIZE;
         i += 2U) {
        if (amiheur_m68k_decode(data + i,
                               AMIHEUR_BOOTBLOCK_SIZE - i,
                               &insn) == 0 &&
            insn.is_a6_lvo &&
            insn.lvo_offset < 0 &&
            amiheur_exec_lvo_lookup(insn.lvo_offset, &info) == 1 &&
            info.classification == classification)
            return 1;
    }
    return 0;
}

static int has_execbase_reference(const unsigned char *data)
{
    unsigned int i;

    for (i = AMIHEUR_BOOT_CODE_OFFSET; i + 3U < AMIHEUR_BOOTBLOCK_SIZE; i += 2U) {
        if (read_be32(data + i) == 4UL)
            return 1;
    }
    return 0;
}

static int has_custom_chip_reference(const unsigned char *data)
{
    unsigned int i;
    unsigned long value;

    for (i = AMIHEUR_BOOT_CODE_OFFSET; i + 3U < AMIHEUR_BOOTBLOCK_SIZE; i += 2U) {
        value = read_be32(data + i);
        if (value >= 0x00dff000UL && value <= 0x00dfffffUL)
            return 1;
    }
    return 0;
}

int amiheur_boot_analyze(const unsigned char *data,
                         size_t size,
                         AmiHeurBootReport *report)
{
    if (data == 0 || report == 0 || size != AMIHEUR_BOOTBLOCK_SIZE)
        return -1;

    memset(report, 0, sizeof(*report));

    report->has_dos_header =
        data[0] == 'D' && data[1] == 'O' && data[2] == 'S';
    report->stored_checksum = read_be32(data + 4U);
    report->computed_checksum = compute_checksum(data);
    report->checksum_valid =
        report->stored_checksum == report->computed_checksum;

    if (!report->has_dos_header)
        add_finding(report, "BOOT.NO_DOS_HEADER", 20);

    if (!report->checksum_valid)
        add_finding(report, "BOOT.BAD_CHECKSUM", 15);

    if (code_present(data))
        add_finding(report, "BOOT.CODE_PRESENT", 0);

    if (has_decoded_kind(data, AMIHEUR_M68K_BRANCH))
        add_finding(report, "BOOT.BRANCH_OPCODE", 5);

    if (has_decoded_kind(data, AMIHEUR_M68K_JSR))
        add_finding(report, "BOOT.JSR_OPCODE", 5);

    if (has_decoded_kind(data, AMIHEUR_M68K_JMP))
        add_finding(report, "BOOT.JMP_OPCODE", 5);

    if (has_decoded_kind(data, AMIHEUR_M68K_RTS))
        add_finding(report, "BOOT.RTS_OPCODE", 0);

    if (has_a6_lvo_call(data))
        add_finding(report, "BOOT.A6_LVO_CALL", 10);

    if (has_exec_lvo_class(data, AMIHEUR_EXEC_LVO_CONTROL))
        add_finding(report, "BOOT.EXEC_CONTROL_LVO", 5);

    if (has_exec_lvo_class(data, AMIHEUR_EXEC_LVO_MUTATION))
        add_finding(report, "BOOT.EXEC_MUTATION_LVO", 20);

    if (has_execbase_reference(data))
        add_finding(report, "BOOT.EXECBASE_REFERENCE", 5);

    if (has_custom_chip_reference(data))
        add_finding(report, "BOOT.CUSTOM_CHIP_REFERENCE", 10);

    return 0;
}
