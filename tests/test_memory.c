#include "amiheuristics/memory.h"
#include <stdio.h>
#include <string.h>

static int finding(const AmiHeurMemoryReport *r, const char *id)
{
    unsigned long i;
    for (i = 0UL; i < r->finding_count && i < AMIHEUR_MEMORY_MAX_FINDINGS; ++i)
        if (strcmp(r->finding_ids[i], id) == 0)
            return 1;
    return 0;
}

static int test_expected(void)
{
    AmiHeurVectorObservation o;
    AmiHeurMemoryReport r;

    o.vector_name = "dos.library/Open";
    o.target = 0x100120UL;
    o.expected_start = 0x100000UL;
    o.expected_end = 0x10ffffUL;

    if (amiheur_memory_analyze_vector(&o, 0, 0U, &r) != 0)
        return 1;
    if (!r.target_in_expected_range || r.score != 0)
        return 1;
    return !finding(&r, "MEM.VECTOR_EXPECTED_RANGE");
}

static int test_unknown(void)
{
    AmiHeurVectorObservation o;
    AmiHeurMemoryReport r;

    o.vector_name = "dos.library/Open";
    o.target = 0x200120UL;
    o.expected_start = 0x100000UL;
    o.expected_end = 0x10ffffUL;

    if (amiheur_memory_analyze_vector(&o, 0, 0U, &r) != 0)
        return 1;
    if (r.target_in_known_region || r.score != 45)
        return 1;
    return !finding(&r, "MEM.VECTOR_UNKNOWN_TARGET");
}

static int test_allowed_patch(void)
{
    AmiHeurVectorObservation o;
    AmiHeurMemoryRegion region;
    AmiHeurMemoryReport r;

    o.vector_name = "exec.library/OpenLibrary";
    o.target = 0x300040UL;
    o.expected_start = 0x100000UL;
    o.expected_end = 0x10ffffUL;

    region.start = 0x300000UL;
    region.end = 0x300fffUL;
    region.owner_kind = AMIHEUR_MEMORY_OWNER_LIBRARY;
    region.owner_name = "knownpatch.library";
    region.executable = 1;
    region.allow_patch = 1;

    if (amiheur_memory_analyze_vector(&o, &region, 1U, &r) != 0)
        return 1;
    if (!r.target_in_known_region || !r.target_allow_patch || r.score != 5)
        return 1;
    return !finding(&r, "MEM.VECTOR_ALLOWED_PATCH");
}

static int test_exec_ram(void)
{
    AmiHeurVectorObservation o;
    AmiHeurMemoryRegion region;
    AmiHeurMemoryReport r;

    o.vector_name = "exec.library/SetFunction";
    o.target = 0x400020UL;
    o.expected_start = 0x100000UL;
    o.expected_end = 0x10ffffUL;

    region.start = 0x400000UL;
    region.end = 0x400fffUL;
    region.owner_kind = AMIHEUR_MEMORY_OWNER_TASK;
    region.owner_name = "unknown-task";
    region.executable = 1;
    region.allow_patch = 0;

    if (amiheur_memory_analyze_vector(&o, &region, 1U, &r) != 0)
        return 1;
    if (!r.target_in_executable_region || r.score != 35)
        return 1;
    return !finding(&r, "MEM.VECTOR_EXECUTABLE_RAM_TARGET");
}

int main(void)
{
    if (test_expected() != 0) { puts("FAIL: expected vector"); return 1; }
    if (test_unknown() != 0) { puts("FAIL: unknown vector target"); return 1; }
    if (test_allowed_patch() != 0) { puts("FAIL: allowed patch"); return 1; }
    if (test_exec_ram() != 0) { puts("FAIL: executable RAM target"); return 1; }
    puts("PASS: memory vector provenance core");
    return 0;
}
