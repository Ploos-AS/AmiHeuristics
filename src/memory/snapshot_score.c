#include "amiheuristics/snapshot_score.h"

static void finding_for_event(const AmiHeurDiffEvent *event,
                              AmiHeurFinding *finding)
{
    finding->domain = AMIHEUR_DOMAIN_MEMORY;
    finding->offset = 0UL;

    switch (event->kind) {
    case AMIHEUR_DIFF_INVENTORY_ADDED:
        finding->rule_id = "MEM.SNAPSHOT_INVENTORY_ADDED";
        finding->severity = AMIHEUR_SEVERITY_LOW;
        finding->score = 5;
        finding->summary = "Runtime inventory item appeared";
        finding->offset = event->new_start;
        break;
    case AMIHEUR_DIFF_INVENTORY_REMOVED:
        finding->rule_id = "MEM.SNAPSHOT_INVENTORY_REMOVED";
        finding->severity = AMIHEUR_SEVERITY_LOW;
        finding->score = 5;
        finding->summary = "Runtime inventory item disappeared";
        finding->offset = event->old_start;
        break;
    case AMIHEUR_DIFF_INVENTORY_CHANGED:
        finding->rule_id = "MEM.SNAPSHOT_INVENTORY_CHANGED";
        finding->severity = AMIHEUR_SEVERITY_MEDIUM;
        finding->score = 10;
        finding->summary = "Runtime inventory range or attributes changed";
        finding->offset = event->new_start;
        break;
    case AMIHEUR_DIFF_VECTOR_ADDED:
        finding->rule_id = "MEM.SNAPSHOT_VECTOR_ADDED";
        finding->severity = AMIHEUR_SEVERITY_LOW;
        finding->score = 5;
        finding->summary = "Observed vector appeared";
        finding->offset = event->new_target;
        break;
    case AMIHEUR_DIFF_VECTOR_REMOVED:
        finding->rule_id = "MEM.SNAPSHOT_VECTOR_REMOVED";
        finding->severity = AMIHEUR_SEVERITY_LOW;
        finding->score = 5;
        finding->summary = "Observed vector disappeared";
        finding->offset = event->old_target;
        break;
    case AMIHEUR_DIFF_VECTOR_RETARGETED:
    default:
        finding->rule_id = "MEM.SNAPSHOT_VECTOR_RETARGETED";
        finding->severity = AMIHEUR_SEVERITY_MEDIUM;
        finding->score = 20;
        finding->summary = "Observed vector target changed";
        finding->offset = event->new_target;
        break;
    }
}

int amiheur_snapshot_score_events(const AmiHeurDiffEvent *events,
                                  size_t event_count,
                                  AmiHeurFinding *findings,
                                  size_t capacity,
                                  size_t *required_count,
                                  AmiHeurScore *score)
{
    size_t i;
    AmiHeurFinding finding;

    if (required_count == NULL || score == NULL) return -1;
    *required_count = event_count;
    if (event_count != 0U && events == NULL) return -1;
    if (event_count > capacity) return -2;
    if (event_count != 0U && findings == NULL) return -1;

    /* Validate enum values before touching caller output or score. */
    for (i = 0U; i < event_count; ++i) {
        if (events[i].kind < AMIHEUR_DIFF_INVENTORY_ADDED ||
            events[i].kind > AMIHEUR_DIFF_VECTOR_RETARGETED)
            return -1;
    }

    amiheur_score_init(score);
    for (i = 0U; i < event_count; ++i) {
        finding_for_event(&events[i], &finding);
        findings[i] = finding;
        if (amiheur_score_add(score, &findings[i]) != 0) return -3;
    }
    return 0;
}
