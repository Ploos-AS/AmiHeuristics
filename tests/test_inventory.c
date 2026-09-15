#include "amiheuristics/inventory.h"
#include <stdio.h>
#include <string.h>

static int test_inventory(void)
{
    AmiHeurInventory inv;
    AmiHeurMemoryRegion regions[4];
    AmiHeurVectorObservation obs;
    AmiHeurMemoryReport report;
    size_t n;

    amiheur_inventory_init(&inv);
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_LIBRARY,
                              "dos.library", 0x10000UL, 0x10fffUL,
                              1, 0) != 0)
        return 1;
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_DEVICE,
                              "trackdisk.device", 0x20000UL, 0x20fffUL,
                              1, 0) != 0)
        return 1;
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_RESIDENT,
                              "known.patch", 0x30000UL, 0x30fffUL,
                              1, 1) != 0)
        return 1;
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_TASK,
                              "worker", 0x40000UL, 0x40fffUL,
                              1, 0) != 0)
        return 1;

    n = amiheur_inventory_regions(&inv, regions, 4U);
    if (n != 4U)
        return 1;
    if (regions[0].owner_kind != AMIHEUR_MEMORY_OWNER_LIBRARY ||
        strcmp(regions[0].owner_name, "dos.library") != 0)
        return 1;
    if (regions[1].owner_kind != AMIHEUR_MEMORY_OWNER_DEVICE)
        return 1;
    if (regions[2].owner_kind != AMIHEUR_MEMORY_OWNER_RESIDENT ||
        !regions[2].allow_patch)
        return 1;
    if (regions[3].owner_kind != AMIHEUR_MEMORY_OWNER_TASK)
        return 1;

    obs.vector_name = "test.vector";
    obs.target = 0x10020UL;
    obs.expected_start = 0x50000UL;
    obs.expected_end = 0x50fffUL;
    if (amiheur_memory_analyze_vector(&obs, regions, n, &report) != 0)
        return 1;
    if (!report.target_in_known_region ||
        report.target_owner_kind != AMIHEUR_MEMORY_OWNER_LIBRARY ||
        strcmp(report.target_owner_name, "dos.library") != 0 ||
        report.score != 20)
        return 1;

    obs.target = 0x30020UL;
    if (amiheur_memory_analyze_vector(&obs, regions, n, &report) != 0)
        return 1;
    if (!report.target_allow_patch || report.score != 5)
        return 1;

    obs.target = 0x40020UL;
    if (amiheur_memory_analyze_vector(&obs, regions, n, &report) != 0)
        return 1;
    if (report.target_owner_kind != AMIHEUR_MEMORY_OWNER_TASK ||
        report.score != 35)
        return 1;

    return 0;
}

static int test_bounds(void)
{
    AmiHeurInventory inv;
    AmiHeurMemoryRegion region;
    size_t i;

    amiheur_inventory_init(&inv);
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_LIBRARY,
                              "bad", 10UL, 9UL, 1, 0) == 0)
        return 1;
    for (i = 0U; i < AMIHEUR_INVENTORY_MAX_ITEMS; ++i)
        if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_TASK,
                                  "task", (unsigned long)i,
                                  (unsigned long)i, 1, 0) != 0)
            return 1;
    if (amiheur_inventory_add(&inv, AMIHEUR_INVENTORY_TASK,
                              "overflow", 0UL, 0UL, 1, 0) == 0)
        return 1;
    if (amiheur_inventory_regions(&inv, &region, 1U) != 1U)
        return 1;
    return 0;
}

int main(void)
{
    if (test_inventory() != 0) {
        puts("FAIL: inventory provenance");
        return 1;
    }
    if (test_bounds() != 0) {
        puts("FAIL: inventory bounds");
        return 1;
    }
    puts("PASS: memory inventory provenance model");
    return 0;
}
