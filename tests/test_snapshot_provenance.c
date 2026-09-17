#include <stdio.h>
#include <string.h>

#include "amiheuristics/snapshot_provenance.h"

int main(void)
{
    AmiHeurInventory inventory;
    AmiHeurDiffEvent events[5];
    AmiHeurFinding findings[5];
    AmiHeurScore score;
    size_t required;
    int rc;

    amiheur_inventory_init(&inventory);
    if (amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_LIBRARY,
                              "dos.library", 0x1000UL, 0x1fffUL, 1, 0) != 0)
        return 1;
    if (amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_RESIDENT,
                              "patcher", 0x2000UL, 0x2fffUL, 1, 1) != 0)
        return 1;
    if (amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_TASK,
                              "worker", 0x3000UL, 0x3fffUL, 1, 0) != 0)
        return 1;
    if (amiheur_inventory_add(&inventory, AMIHEUR_INVENTORY_TASK,
                              "data", 0x4000UL, 0x4fffUL, 0, 0) != 0)
        return 1;

    memset(events, 0, sizeof(events));
    events[0].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[0].new_target = 0x1100UL;
    events[1].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[1].new_target = 0x2100UL;
    events[2].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[2].new_target = 0x3100UL;
    events[3].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[3].new_target = 0x4100UL;
    events[4].kind = AMIHEUR_DIFF_VECTOR_RETARGETED;
    events[4].new_target = 0x9000UL;

    required = 0U;
    rc = amiheur_snapshot_correlate_provenance(events, 5U, &inventory,
                                                findings, 5U, &required,
                                                &score);
    if (rc != 0 || required != 5U || score.total != 80L) return 1;
    if (strcmp(findings[0].rule_id, "MEM.CORR_RETARGET_KNOWN_MODULE") != 0 ||
        findings[0].score != 5) return 1;
    if (strcmp(findings[1].rule_id, "MEM.CORR_RETARGET_ALLOWED") != 0 ||
        findings[1].score != 0) return 1;
    if (strcmp(findings[2].rule_id, "MEM.CORR_RETARGET_EXEC_RAM") != 0 ||
        findings[2].score != 20) return 1;
    if (strcmp(findings[3].rule_id, "MEM.CORR_RETARGET_NONEXEC") != 0 ||
        findings[3].score != 25) return 1;
    if (strcmp(findings[4].rule_id, "MEM.CORR_RETARGET_UNKNOWN") != 0 ||
        findings[4].score != 30) return 1;

    required = 0U;
    if (amiheur_snapshot_correlate_provenance(events, 5U, &inventory,
                                               findings, 4U, &required,
                                               &score) != -2 ||
        required != 5U) return 1;

    puts("PASS: snapshot provenance correlation");
    return 0;
}
