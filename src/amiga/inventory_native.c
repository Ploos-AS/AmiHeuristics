#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/execbase.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <exec/resident.h>
#include <proto/exec.h>

#include "amiheuristics/amiga_inventory.h"

extern struct ExecBase *SysBase;

static int collect_list(struct List *list, AmiHeurInventoryKind kind,
                        AmiHeurInventory *inventory)
{
    struct Node *node;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ) {
        const char *name = node->ln_Name != NULL ? node->ln_Name : "<unnamed>";
        unsigned long address = (unsigned long)node;
        if (amiheur_inventory_add(inventory, kind, name, address, address,
                                  1, 0) != 0)
            return -2;
    }
    return 0;
}

int amiheur_amiga_inventory_collect(AmiHeurInventory *inventory)
{
    int rc = 0;

    if (inventory == NULL || SysBase == NULL) return -1;
    amiheur_inventory_init(inventory);

    /*
     * Keep the protected section short: no allocation, I/O or reporting.
     * Names remain borrowed Exec pointers, so callers must treat the result
     * as a short-lived snapshot until a later owned-name layer is added.
     */
    Forbid();
    if (collect_list(&SysBase->LibList, AMIHEUR_INVENTORY_LIBRARY, inventory) != 0)
        rc = -2;
    else if (collect_list(&SysBase->DeviceList, AMIHEUR_INVENTORY_DEVICE, inventory) != 0)
        rc = -2;
    else if (collect_list(&SysBase->TaskReady, AMIHEUR_INVENTORY_TASK, inventory) != 0)
        rc = -2;
    else if (collect_list(&SysBase->TaskWait, AMIHEUR_INVENTORY_TASK, inventory) != 0)
        rc = -2;
    Permit();

    return rc;
}

#endif
