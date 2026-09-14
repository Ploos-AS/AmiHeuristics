#include "amiheuristics/bootblock.h"
#include <string.h>

static unsigned long read_be32(const unsigned char *p)
{
    return ((unsigned long)p[0] << 24) |
           ((unsigned long)p[1] << 16) |
           ((unsigned long)p[2] << 8) |
           (unsigned long)p[3];
}

static unsigned long compute_checksum(const unsigned char *data)
{
    unsigned long sum;
    unsigned long value;
    unsigned long old_sum;
    unsigned int i;

    sum = 0UL;
    for (i = 0U; i < 256U; ++i) {
        value = (i == 1U) ? 0UL : read_be32(data + (i * 4U));
        old_sum = sum;
        sum += value;
        if (sum < old_sum)
            ++sum;
    }
    return ~sum;
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

    return 0;
}
