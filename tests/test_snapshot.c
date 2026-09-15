#include <stdio.h>
#include <string.h>
#include "amiheuristics/snapshot.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static int has_event(const AmiHeurDiffEvent *events, size_t count,
                     AmiHeurDiffKind kind, const char *name)
{
    size_t i;
    for (i = 0U; i < count; ++i) {
        if (events[i].kind == kind && strcmp(events[i].name, name) == 0) return 1;
    }
    return 0;
}

int main(void)
{
    AmiHeurInventoryItem before_items[3];
    AmiHeurInventoryItem after_items[3];
    AmiHeurVectorState before_vectors[2];
    AmiHeurVectorState after_vectors[2];
    AmiHeurSnapshotView before;
    AmiHeurSnapshotView after;
    AmiHeurDiffEvent events[8];
    size_t count;
    int rc;

    before_items[0].kind = AMIHEUR_INVENTORY_LIBRARY;
    before_items[0].name = "dos.library";
    before_items[0].start = 0x10000UL; before_items[0].end = 0x10fffUL;
    before_items[0].executable = 1; before_items[0].allow_patch = 0;
    before_items[1].kind = AMIHEUR_INVENTORY_TASK;
    before_items[1].name = "worker";
    before_items[1].start = 0x20000UL; before_items[1].end = 0x20fffUL;
    before_items[1].executable = 1; before_items[1].allow_patch = 0;
    before_items[2].kind = AMIHEUR_INVENTORY_RESIDENT;
    before_items[2].name = "known.patch";
    before_items[2].start = 0x30000UL; before_items[2].end = 0x30fffUL;
    before_items[2].executable = 1; before_items[2].allow_patch = 1;

    after_items[0] = before_items[2];
    after_items[1] = before_items[0];
    after_items[1].end = 0x11fffUL;
    after_items[2].kind = AMIHEUR_INVENTORY_DEVICE;
    after_items[2].name = "trackdisk.device";
    after_items[2].start = 0x40000UL; after_items[2].end = 0x40fffUL;
    after_items[2].executable = 1; after_items[2].allow_patch = 0;

    before_vectors[0].name = "dos.library/Open"; before_vectors[0].target = 0x10100UL;
    before_vectors[1].name = "exec.library/SetFunction"; before_vectors[1].target = 0x50100UL;
    after_vectors[0].name = "exec.library/SetFunction"; after_vectors[0].target = 0x60100UL;
    after_vectors[1].name = "dos.library/Open"; after_vectors[1].target = 0x10100UL;

    before.items = before_items; before.item_count = 3U;
    before.vectors = before_vectors; before.vector_count = 2U;
    after.items = after_items; after.item_count = 3U;
    after.vectors = after_vectors; after.vector_count = 2U;

    rc = amiheur_snapshot_diff(&before, &after, events, 8U, &count);
    CHECK(rc == 0);
    CHECK(count == 4U);
    CHECK(has_event(events, count, AMIHEUR_DIFF_INVENTORY_REMOVED, "worker"));
    CHECK(has_event(events, count, AMIHEUR_DIFF_INVENTORY_CHANGED, "dos.library"));
    CHECK(has_event(events, count, AMIHEUR_DIFF_INVENTORY_ADDED, "trackdisk.device"));
    CHECK(has_event(events, count, AMIHEUR_DIFF_VECTOR_RETARGETED, "exec.library/SetFunction"));

    rc = amiheur_snapshot_diff(&before, &after, events, 2U, &count);
    CHECK(rc == -2);
    CHECK(count == 4U);

    rc = amiheur_snapshot_diff(&before, &before, events, 8U, &count);
    CHECK(rc == 0);
    CHECK(count == 0U);

    printf("PASS: memory snapshot diff model\n");
    return 0;
}
