#ifndef AMIHEURISTICS_SNAPSHOT_PROVENANCE_H
#define AMIHEURISTICS_SNAPSHOT_PROVENANCE_H

#include <stddef.h>
#include "amiheuristics/amiheuristics.h"
#include "amiheuristics/inventory.h"
#include "amiheuristics/snapshot.h"

/* Correlate vector retarget events with ownership of the new target.
 * Emits one additional finding per retarget event. This supplements, rather
 * than replaces, the base snapshot score. */
int amiheur_snapshot_correlate_provenance(
    const AmiHeurDiffEvent *events,
    size_t event_count,
    const AmiHeurInventory *after_inventory,
    AmiHeurFinding *findings,
    size_t capacity,
    size_t *required_count,
    AmiHeurScore *score);

#endif
