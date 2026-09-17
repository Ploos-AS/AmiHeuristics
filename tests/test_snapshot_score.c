#include <stdio.h>
#include <string.h>

#include "amiheuristics/snapshot_score.h"

int main(void)
{
    AmiHeurDiffEvent events[3];
    AmiHeurFinding findings[3];
    AmiHeurScore score;
    size_t required;
    int rc;

    memset(events, 0, sizeof(events));
    events[0].kind = AMIHEUR_DIFF_INVENTORY_ADDED;
    events[0].name = "patcher";
    events[0].new_start = 0x1000UL;
    events[1].kind = AMIHEUR_DIFF_INVENTORY_CHANGED;
    events[1].name = "dos.library";
    events[1].new_start = 0x2000UL;
    events[2].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[2].name = "dos.library/Open";
    events[2].old_target = 0x2100UL;
    events[2].new_target = 0x3100UL;

    required = 0U;
    rc = amiheur_snapshot_score_events(events, 3U, findings, 3U,
                                        &required, &score);
    if (rc != 0 || required != 3U || score.total != 35L ||
        score.finding_count != 3UL) return 1;
    if (strcmp(findings[0].rule_id, "MEM.SNAPSHOT_INVENTORY_ADDED") != 0 ||
        findings[0].score != 5 || findings[0].offset != 0x1000UL) return 1;
    if (strcmp(findings[1].rule_id, "MEM.SNAPSHOT_INVENTORY_CHANGED") != 0 ||
        findings[1].score != 10) return 1;
    if (strcmp(findings[2].rule_id, "MEM.SNAPSHOT_VECTOR_RETARGETED") != 0 ||
        findings[2].score != 20 || findings[2].offset != 0x3100UL) return 1;

    required = 0U;
    if (amiheur_snapshot_score_events(events, 3U, findings, 2U,
                                      &required, &score) != -2 ||
        required != 3U) return 1;

    events[1].kind = (AmiHeurDiffKind)99;
    findings[0].score = 1234;
    score.total = 5678L;
    if (amiheur_snapshot_score_events(events, 3U, findings, 3U,
                                      &required, &score) != -1) return 1;
    if (findings[0].score != 1234 || score.total != 5678L) return 1;

    puts("PASS: snapshot diff scoring");
    return 0;
}
