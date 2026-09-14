#include "amiheuristics/bootblock.h"

#include <stdio.h>
#include <string.h>

static void write_be32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)((v >> 24) & 0xffUL);
    p[1] = (unsigned char)((v >> 16) & 0xffUL);
    p[2] = (unsigned char)((v >> 8) & 0xffUL);
    p[3] = (unsigned char)(v & 0xffUL);
}

static unsigned long read_be32(const unsigned char *p)
{
    return ((unsigned long)p[0] << 24) |
           ((unsigned long)p[1] << 16) |
           ((unsigned long)p[2] << 8) |
           (unsigned long)p[3];
}

static unsigned long checksum(const unsigned char *data)
{
    unsigned long sum;
    unsigned long value;
    unsigned long old_sum;
    unsigned int i;

    sum = 0UL;
    for (i = 0U; i < 256U; ++i) {
        value = (i == 1U) ? 0UL : read_be32(data + i * 4U);
        old_sum = sum;
        sum += value;
        if (sum < old_sum)
            ++sum;
    }
    return ~sum;
}

static int test_valid(void)
{
    unsigned char block[AMIHEUR_BOOTBLOCK_SIZE];
    AmiHeurBootReport report;

    memset(block, 0, sizeof(block));
    block[0] = 'D';
    block[1] = 'O';
    block[2] = 'S';
    block[3] = 0;
    write_be32(block + 4U, checksum(block));

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
    puts("PASS: bootblock validation");
    return 0;
}
