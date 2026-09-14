#ifndef AMIHEURISTICS_BOOTBLOCK_H
#define AMIHEURISTICS_BOOTBLOCK_H

#include <stddef.h>

#define AMIHEUR_BOOTBLOCK_SIZE 1024U
#define AMIHEUR_BOOT_MAX_FINDINGS 16U

typedef struct AmiHeurBootReport {
    int has_dos_header;
    int checksum_valid;
    unsigned long stored_checksum;
    unsigned long computed_checksum;
    int score;
    unsigned long finding_count;
    const char *finding_ids[AMIHEUR_BOOT_MAX_FINDINGS];
} AmiHeurBootReport;

int amiheur_boot_analyze(const unsigned char *data, size_t size,
                         AmiHeurBootReport *report);

#endif
