#ifndef AMIHEURISTICS_SNAPSHOT_H
#define AMIHEURISTICS_SNAPSHOT_H

#include <stddef.h>
#include "amiheuristics/inventory.h"

typedef struct AmiHeurVectorState {
    const char *name;
    unsigned long target;
} AmiHeurVectorState;

typedef struct AmiHeurSnapshotView {
    const AmiHeurInventoryItem *items;
    size_t item_count;
    const AmiHeurVectorState *vectors;
    size_t vector_count;
} AmiHeurSnapshotView;

typedef enum AmiHeurDiffKind {
    AMIHEUR_DIFF_INVENTORY_ADDED = 0,
    AMIHEUR_DIFF_INVENTORY_REMOVED,
    AMIHEUR_DIFF_INVENTORY_CHANGED,
    AMIHEUR_DIFF_VECTOR_ADDED,
    AMIHEUR_DIFF_VECTOR_REMOVED,
    AMIHEUR_DIFF_VECTOR_RETARGETED
} AmiHeurDiffKind;

typedef struct AmiHeurDiffEvent {
    AmiHeurDiffKind kind;
    const char *name;
    AmiHeurInventoryKind inventory_kind;
    unsigned long old_start;
    unsigned long old_end;
    unsigned long new_start;
    unsigned long new_end;
    unsigned long old_target;
    unsigned long new_target;
} AmiHeurDiffEvent;

/*
 * Diff two caller-owned snapshots without allocating memory.
 * Returns 0 on success, -1 for invalid input, or -2 if capacity is too small.
 * required_count is always set on valid input so callers can size buffers.
 */
int amiheur_snapshot_diff(const AmiHeurSnapshotView *before,
                          const AmiHeurSnapshotView *after,
                          AmiHeurDiffEvent *events,
                          size_t capacity,
                          size_t *required_count);

#endif
