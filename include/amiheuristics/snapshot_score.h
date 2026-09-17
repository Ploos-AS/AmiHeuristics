#ifndef AMIHEURISTICS_SNAPSHOT_SCORE_H
#define AMIHEURISTICS_SNAPSHOT_SCORE_H

#include <stddef.h>
#include "amiheuristics/amiheuristics.h"
#include "amiheuristics/snapshot.h"

/* Convert snapshot changes into explainable memory-domain findings.
 * This is deliberately conservative: additions/removals are low evidence,
 * while vector retargeting is stronger but still not malware by itself. */
int amiheur_snapshot_score_events(const AmiHeurDiffEvent *events,
                                  size_t event_count,
                                  AmiHeurFinding *findings,
                                  size_t capacity,
                                  size_t *required_count,
                                  AmiHeurScore *score);

#endif
