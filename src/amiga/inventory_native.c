#ifdef AMIHEURISTICS_AMIGA_NATIVE

#include <exec/execbase.h>
#include <exec/libraries.h>
#include <exec/lists.h>
#include <exec/nodes.h>
#include <proto/exec.h>

#include "amiheuristics/amiga_inventory.h"

extern struct ExecBase *SysBase;

static int collect_library_list(struct List *list, AmiHeurInventoryKind kind,
                                AmiHeurInventory *inventory)
{
    struct Node *node;

    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ) {
        struct Library *library = (struct Library *)node;
        const char *name = node->ln_Name != NULL ? node->ln_Name : "<unnamed>";
        unsigned long base = (unsigned long)library;
        unsigned long neg = (unsigned long)library->lib_NegSize;
        unsigned long pos = (unsigned long)library->lib_PosSize;
        unsigned long start;
        unsigned long end;

        if (base < neg || pos == 0UL || base > ~0UL - (pos - 1UL))
            return -3;
        start = base - neg;
        end = base + pos - 1UL;
        /* This is the library/device allocation containing its vectors and
         * base structure. It is executable-capable provenance, not a claim
         * that every byte in the allocation is code. */
        if (amiheur_inventory_add(inventory, kind, name, start, end, 1, 0) != 0)
            return -2;
    }
    return 0;
}

static int collect_task_list(struct List *list, AmiHeurInventory *inventory)
{
    struct Node *node;
    for (node = list->lh_Head; node->ln_Succ != NULL; node = node->ln_Succ) {
        const char *name = node->ln_Name != NULL ? node->ln_Name : "<unnamed>";
        unsigned long address = (unsigned long)node;
        /* A task node does not describe the task's executable code extent.
         * Keep it for inventory/diff purposes but mark it non-executable so
         * vector provenance cannot mistake the node for owned code. */
        if (amiheur_inventory_add(inventory, AMIHEUR_INVENTORY_TASK,
                                  name, address, address, 0, 0) != 0)
            return -2;
    }
    return 0;
}

int amiheur_amiga_inventory_collect(AmiHeurInventory *inventory)
{
    int rc = 0;

    if (inventory == NULL || SysBase == NULL) return -1;
    amiheur_inventory_init(inventory);

    Forbid();
    rc = collect_library_list(&SysBase->LibList, AMIHEUR_INVENTORY_LIBRARY,
                              inventory);
    if (rc == 0)
        rc = collect_library_list(&SysBase->DeviceList, AMIHEUR_INVENTORY_DEVICE,
                                  inventory);
    if (rc == 0)
        rc = collect_task_list(&SysBase->TaskReady, inventory);
    if (rc == 0)
        rc = collect_task_list(&SysBase->TaskWait, inventory);
    Permit();

    return rc;
}

#endif
