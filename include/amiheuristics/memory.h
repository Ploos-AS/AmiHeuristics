#ifndef AMIHEURISTICS_MEMORY_H
#define AMIHEURISTICS_MEMORY_H

#include <stddef.h>

#define AMIHEUR_MEMORY_MAX_FINDINGS 16U

typedef enum AmiHeurMemoryOwnerKind {
    AMIHEUR_MEMORY_OWNER_UNKNOWN = 0,
    AMIHEUR_MEMORY_OWNER_ROM,
    AMIHEUR_MEMORY_OWNER_LIBRARY,
    AMIHEUR_MEMORY_OWNER_DEVICE,
    AMIHEUR_MEMORY_OWNER_RESIDENT,
    AMIHEUR_MEMORY_OWNER_TASK,
    AMIHEUR_MEMORY_OWNER_OTHER
} AmiHeurMemoryOwnerKind;

typedef struct AmiHeurMemoryRegion {
    unsigned long start;
    unsigned long end;
    AmiHeurMemoryOwnerKind owner_kind;
    const char *owner_name;
    int executable;
    int allow_patch;
} AmiHeurMemoryRegion;

typedef struct AmiHeurVectorObservation {
    const char *vector_name;
    unsigned long target;
    unsigned long expected_start;
    unsigned long expected_end;
} AmiHeurVectorObservation;

typedef struct AmiHeurMemoryReport {
    int target_in_expected_range;
    int target_in_known_region;
    int target_in_executable_region;
    int target_allow_patch;
    AmiHeurMemoryOwnerKind target_owner_kind;
    const char *target_owner_name;
    int score;
    unsigned long finding_count;
    const char *finding_ids[AMIHEUR_MEMORY_MAX_FINDINGS];
} AmiHeurMemoryReport;

int amiheur_memory_analyze_vector(const AmiHeurVectorObservation *obs,
                                  const AmiHeurMemoryRegion *regions,
                                  size_t region_count,
                                  AmiHeurMemoryReport *report);

#endif
