#include "amiheuristics/memory.h"
#include <string.h>

static void finding(AmiHeurMemoryReport *r, const char *id, int weight)
{
    if (r->finding_count < AMIHEUR_MEMORY_MAX_FINDINGS)
        r->finding_ids[r->finding_count] = id;
    ++r->finding_count;
    r->score += weight;
}

static int in_range(unsigned long value, unsigned long start,
                    unsigned long end)
{
    return start <= end && value >= start && value <= end;
}

int amiheur_memory_analyze_vector(const AmiHeurVectorObservation *obs,
                                  const AmiHeurMemoryRegion *regions,
                                  size_t region_count,
                                  AmiHeurMemoryReport *report)
{
    size_t i;

    if (obs == 0 || report == 0 ||
        (regions == 0 && region_count != 0U))
        return -1;

    memset(report, 0, sizeof(*report));
    report->target_owner_kind = AMIHEUR_MEMORY_OWNER_UNKNOWN;

    if (in_range(obs->target, obs->expected_start, obs->expected_end)) {
        report->target_in_expected_range = 1;
        finding(report, "MEM.VECTOR_EXPECTED_RANGE", 0);
        return 0;
    }

    for (i = 0U; i < region_count; ++i) {
        if (!in_range(obs->target, regions[i].start, regions[i].end))
            continue;
        report->target_in_known_region = 1;
        report->target_in_executable_region = regions[i].executable;
        report->target_allow_patch = regions[i].allow_patch;
        report->target_owner_kind = regions[i].owner_kind;
        report->target_owner_name = regions[i].owner_name;
        break;
    }

    finding(report, "MEM.VECTOR_OUTSIDE_EXPECTED", 15);

    if (!report->target_in_known_region) {
        finding(report, "MEM.VECTOR_UNKNOWN_TARGET", 30);
        return 0;
    }

    if (report->target_allow_patch) {
        finding(report, "MEM.VECTOR_ALLOWED_PATCH", -10);
        return 0;
    }

    if (!report->target_in_executable_region) {
        finding(report, "MEM.VECTOR_NONEXEC_TARGET", 25);
        return 0;
    }

    if (report->target_owner_kind == AMIHEUR_MEMORY_OWNER_ROM)
        finding(report, "MEM.VECTOR_ROM_TARGET", 0);
    else if (report->target_owner_kind == AMIHEUR_MEMORY_OWNER_LIBRARY ||
             report->target_owner_kind == AMIHEUR_MEMORY_OWNER_DEVICE ||
             report->target_owner_kind == AMIHEUR_MEMORY_OWNER_RESIDENT)
        finding(report, "MEM.VECTOR_KNOWN_MODULE_TARGET", 5);
    else
        finding(report, "MEM.VECTOR_EXECUTABLE_RAM_TARGET", 20);

    return 0;
}
