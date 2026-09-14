#include "amiheuristics/bootblock.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>

#define TEST_U32_MASK 0xffffffffUL

static void write_be32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)((v >> 24) & 0xffUL);
    p[1] = (unsigned char)((v >> 16) & 0xffUL);
    p[2] = (unsigned char)((v >> 8) & 0xffUL);
    p[3] = (unsigned char)(v & 0xffUL);
}

static unsigned long read_be32(const unsigned char *p)
{
    return (((unsigned long)p[0] << 24) |
            ((unsigned long)p[1] << 16) |
            ((unsigned long)p[2] << 8) |
            (unsigned long)p[3]) & TEST_U32_MASK;
}

static unsigned long add_u32_carry(unsigned long sum, unsigned long value)
{
#if ULONG_MAX > 0xffffffffUL
    unsigned long total;

    total = (sum & TEST_U32_MASK) + (value & TEST_U32_MASK);
    sum = total & TEST_U32_MASK;
    if (total > TEST_U32_MASK)
        sum = (sum + 1UL) & TEST_U32_MASK;
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

static unsigned long checksum(const unsigned char *data)
{
    unsigned long sum;
    unsigned long value;
    unsigned int i;

    sum = 0UL;
    for (i = 0U; i < 256U; ++i) {
        value = (i == 1U) ? 0UL : read_be32(data + i * 4U);
        sum = add_u32_carry(sum, value);
    }
    return (~sum) & TEST_U32_MASK;
}

static int has_finding(const AmiHeurBootReport *report, const char *id)
{
    unsigned long i;

    for (i = 0UL; i < report->finding_count && i < AMIHEUR_BOOT_MAX_FINDINGS; ++i) {
        if (strcmp(report->finding_ids[i], id) == 0)
            return 1;
    }
    return 0;
}

static void make_dos_block(unsigned char *block)
{
    memset(block, 0, AMIHEUR_BOOTBLOCK_SIZE);
    block[0] = 'D';
    block[1] = 'O';
    block[2] = 'S';
    block[3] = 0;
}

static void seal_checksum(unsigned char *block)
{
    write_be32(block + 4U, 0UL);
    write_be32(block + 4U, checksum(block));
}

static void put_jsr_a6(unsigned char *p, int offset)
{
    unsigned int word;

    word = (unsigned int)(offset & 0xffff);
    p[0] = 0x4eU;
    p[1] = 0xaeU;
    p[2] = (unsigned char)((word >> 8) & 0xffU);
    p[3] = (unsigned char)(word & 0xffU);
}

static int test_valid(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    make_dos_block(block);
    seal_checksum(block);

    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (!report.has_dos_header || !report.checksum_valid)
        return 1;
    if (report.score != 0 || report.finding_count != 0UL)
        return 1;
    return 0;
}

static int test_invalid(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    memset(block, 0, sizeof(block));
    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (report.has_dos_header || report.checksum_valid)
        return 1;
    if (report.score != 35 || report.finding_count != 2UL)
        return 1;
    return 0;
}

static int test_code_heuristics(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    make_dos_block(block);

    block[12] = 0x60U;
    block[13] = 0x02U;
    block[14] = 0x4eU;
    block[15] = 0x71U;
    write_be32(block + 16U, 4UL);
    write_be32(block + 20U, 0x00dff180UL);
    block[24] = 0x12U;
    block[25] = 0x34U;
    block[26] = 0x56U;
    block[27] = 0x78U;
    seal_checksum(block);

    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (!report.checksum_valid)
        return 1;
    if (!has_finding(&report, "BOOT.CODE_PRESENT"))
        return 1;
    if (!has_finding(&report, "BOOT.BRANCH_OPCODE"))
        return 1;
    if (!has_finding(&report, "BOOT.EXECBASE_REFERENCE"))
        return 1;
    if (!has_finding(&report, "BOOT.CUSTOM_CHIP_REFERENCE"))
        return 1;
    if (report.score != 20 || report.finding_count != 4UL)
        return 1;
    return 0;
}

static int test_lvo_heuristic(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    make_dos_block(block);
    put_jsr_a6(block + 12U, -60);
    block[16] = 0x4eU;
    block[17] = 0x75U;
    block[18] = 0x12U;
    block[19] = 0x34U;
    block[20] = 0x56U;
    block[21] = 0x78U;
    seal_checksum(block);

    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (!report.checksum_valid)
        return 1;
    if (!has_finding(&report, "BOOT.JSR_OPCODE"))
        return 1;
    if (!has_finding(&report, "BOOT.A6_LVO_CALL"))
        return 1;
    if (!has_finding(&report, "BOOT.RTS_OPCODE"))
        return 1;
    if (has_finding(&report, "BOOT.EXEC_MUTATION_LVO"))
        return 1;
    if (report.score != 15)
        return 1;
    return 0;
}

static int test_exec_mutation_lvo(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    make_dos_block(block);
    put_jsr_a6(block + 12U, -420);
    block[16] = 0x4eU;
    block[17] = 0x75U;
    block[18] = 0x12U;
    block[19] = 0x34U;
    block[20] = 0x56U;
    block[21] = 0x78U;
    seal_checksum(block);

    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (!has_finding(&report, "BOOT.A6_LVO_CALL"))
        return 1;
    if (!has_finding(&report, "BOOT.EXEC_MUTATION_LVO"))
        return 1;
    if (has_finding(&report, "BOOT.EXEC_CONTROL_LVO"))
        return 1;
    if (report.score != 35)
        return 1;
    return 0;
}

static int test_exec_control_lvo(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    make_dos_block(block);
    put_jsr_a6(block + 12U, -132);
    block[16] = 0x4eU;
    block[17] = 0x75U;
    block[18] = 0x12U;
    block[19] = 0x34U;
    block[20] = 0x56U;
    block[21] = 0x78U;
    seal_checksum(block);

    if (amiheur_boot_analyze(block, sizeof(block), &report) != 0)
        return 1;
    if (!has_finding(&report, "BOOT.EXEC_CONTROL_LVO"))
        return 1;
    if (has_finding(&report, "BOOT.EXEC_MUTATION_LVO"))
        return 1;
    if (report.score != 20)
        return 1;
    return 0;
}

int main(void)
{
    if (test_valid() != 0) {
        puts("FAIL: valid bootblock");
        return 1;
    }
    if (test_invalid() != 0) {
        puts("FAIL: invalid bootblock");
        return 1;
    }
    if (test_code_heuristics() != 0) {
        puts("FAIL: bootblock code heuristics");
        return 1;
    }
    if (test_lvo_heuristic() != 0) {
        puts("FAIL: bootblock A6 LVO heuristic");
        return 1;
    }
    if (test_exec_mutation_lvo() != 0) {
        puts("FAIL: Exec mutation LVO heuristic");
        return 1;
    }
    if (test_exec_control_lvo() != 0) {
        puts("FAIL: Exec control LVO heuristic");
        return 1;
    }
    puts("PASS: bootblock validation and Exec LVO semantics");
    return 0;
}
