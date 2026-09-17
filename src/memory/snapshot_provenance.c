#include "amiheuristics/snapshot_provenance.h"

static void provenance_finding(const AmiHeurDiffEvent *event,
                               const AmiHeurInventory *inventory,
                               AmiHeurFinding *finding)
{
    AmiHeurMemoryRegion region;
    int owned;

    finding->domain = AMIHEUR_DOMAIN_MEMORY;
    finding->offset = event->new_target;
    owned = amiheur_inventory_find_region(inventory, event->new_target,
                                          &region);

    if (owned == 0) {
        finding->rule_id = "MEM.CORR_RETARGET_UNKNOWN";
        finding->severity = AMIHEUR_SEVERITY_HIGH;
        finding->score = 30;
        finding->summary = "Vector retargeted to memory with unknown owner";
    } else if (!region.executable) {
        finding->rule_id = "MEM.CORR_RETARGET_NONEXEC";
        finding->severity = AMIHEUR_SEVERITY_HIGH;
        finding->score = 25;
        finding->summary = "Vector retargeted to known non-executable region";
    } else if (region.allow_patch) {
        finding->rule_id = "MEM.CORR_RETARGET_ALLOWED";
        finding->severity = AMIHEUR_SEVERITY_INFO;
        finding->score = 0;
        finding->summary = "Vector retargeted to allowlisted patch owner";
    } else if (region.owner_kind == AMIHEUR_MEMORY_OWNER_LIBRARY ||
               region.owner_kind == AMIHEUR_MEMORY_OWNER_DEVICE ||
               region.owner_kind == AMIHEUR_MEMORY_OWNER_RESIDENT) {
        finding->rule_id = "MEM.CORR_RETARGET_KNOWN_MODULE";
        finding->severity = AMIHEUR_SEVERITY_LOW;
        finding->score = 5;
        finding->summary = "Vector retargeted to known executable module";
    } else {
        finding->rule_id = "MEM.CORR_RETARGET_EXEC_RAM";
        finding->severity = AMIHEUR_SEVERITY_MEDIUM;
        finding->score = 20;
        finding->summary = "Vector retargeted to executable runtime memory";
    }
}

int amiheur_snapshot_correlate_provenance(
    const AmiHeurDiffEvent *events,
    size_t event_count,
    const AmiHeurInventory *after_inventory,
    AmiHeurFinding *findings,
    size_t capacity,
    size_t *required_count,
    AmiHeurScore *score)
{
    size_t i;
    size_t needed;
    size_t out;
    AmiHeurMemoryRegion region;

    if (required_count == NULL || score == NULL || after_inventory == NULL)
        return -1;
    if (event_count != 0U && events == NULL) return -1;
    if (after_inventory->count > AMIHEUR_INVENTORY_MAX_ITEMS) return -1;

    needed = 0U;
    for (i = 0U; i < event_count; ++i) {
        if (events[i].kind < AMIHEUR_DIFF_INVENTORY_ADDED ||
            events[i].kind > AMIHEUR_DIFF_VECTOR_RETARGETED)
            return -1;
        if (events[i].kind == AMIHEUR_DIFF_VECTOR_RETARGETED) {
            if (amiheur_inventory_find_region(after_inventory,
                                              events[i].new_target,
                                              &region) < 0)
                return -1;
            ++needed;
        }
    }

    *required_count = needed;
    if (needed > capacity) return -2;
    if (needed != 0U && findings == NULL) return -1;

    amiheur_score_init(score);
    out = 0U;
    for (i = 0U; i < event_count; ++i) {
        if (events[i].kind != AMIHEUR_DIFF_VECTOR_RETARGETED) continue;
        provenance_finding(&events[i], after_inventory, &findings[out]);
        if (amiheur_score_add(score, &findings[out]) != 0) return -3;
        ++out;
    }
    return 0;
}
