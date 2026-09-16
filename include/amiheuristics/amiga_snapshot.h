#ifndef AMIHEURISTICS_AMIGA_SNAPSHOT_H
#define AMIHEURISTICS_AMIGA_SNAPSHOT_H

#include <stddef.h>
#include "amiheuristics/inventory.h"
#include "amiheuristics/snapshot.h"

#define AMIHEUR_AMIGA_SNAPSHOT_MAX_VECTORS 128U

typedef struct AmiHeurAmigaSnapshot {
    AmiHeurInventory inventory;
    AmiHeurVectorState vectors[AMIHEUR_AMIGA_SNAPSHOT_MAX_VECTORS];
    size_t vector_count;
} AmiHeurAmigaSnapshot;

#ifdef AMIHEURISTICS_AMIGA_NATIVE
struct Library;

/* Capture conservative native inventory plus a caller-selected vector table.
 * Names are borrowed from the caller and must outlive use of the snapshot.
 * All vector slots are validated before the snapshot is committed. */
int amiheur_amiga_snapshot_capture(const struct Library *base,
                                   const char *const *vector_names,
                                   size_t vector_count,
                                   AmiHeurAmigaSnapshot *snapshot);
#endif

/* Produce a portable view suitable for amiheur_snapshot_diff(). */
int amiheur_amiga_snapshot_view(const AmiHeurAmigaSnapshot *snapshot,
                                AmiHeurSnapshotView *view);

#endif
