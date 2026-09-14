#ifndef AMIHEURISTICS_HUNK_H
#define AMIHEURISTICS_HUNK_H

#include <stddef.h>

#define AMIHEUR_HUNK_MAX_FINDINGS 16U

typedef struct AmiHeurHunkReport {
    int valid;
    unsigned long hunk_count;
    unsigned long code_bytes;
    unsigned long data_bytes;
    unsigned long bss_bytes;
    unsigned long relocation_hunks;
    int has_code;
    int has_data;
    int has_bss;
    int has_a6_lvo_call;
    int has_exec_control_lvo;
    int has_exec_mutation_lvo;
    unsigned long exec_control_lvo_count;
    unsigned long exec_mutation_lvo_count;
    int score;
    unsigned long finding_count;
    const char *finding_ids[AMIHEUR_HUNK_MAX_FINDINGS];
} AmiHeurHunkReport;

int amiheur_hunk_analyze(const unsigned char *data, size_t size,
                         AmiHeurHunkReport *report);

#endif
