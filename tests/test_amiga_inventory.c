#include <stdio.h>
#include <string.h>
#include "amiheuristics/amiga_inventory.h"

#define CHECK(x) do { if (!(x)) { printf("FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    AmiHeurAmigaNodeView nodes[4];
    AmiHeurInventory inventory;
    int rc;

    nodes[0].kind = AMIHEUR_INVENTORY_LIBRARY;
    nodes[0].name = "dos.library";
    nodes[0].address = 0x10000UL;
    nodes[1].kind = AMIHEUR_INVENTORY_DEVICE;
    nodes[1].name = "trackdisk.device";
    nodes[1].address = 0x20000UL;
    nodes[2].kind = AMIHEUR_INVENTORY_TASK;
    nodes[2].name = "Workbench";
    nodes[2].address = 0x30000UL;
    nodes[3].kind = AMIHEUR_INVENTORY_RESIDENT;
    nodes[3].name = "ramlib";
    nodes[3].address = 0x40000UL;

    rc = amiheur_amiga_inventory_collect_views(nodes, 4U, &inventory);
    CHECK(rc == 0);
    CHECK(inventory.count == 4U);
    CHECK(inventory.items[0].kind == AMIHEUR_INVENTORY_LIBRARY);
    CHECK(strcmp(inventory.items[0].name, "dos.library") == 0);
    CHECK(inventory.items[1].kind == AMIHEUR_INVENTORY_DEVICE);
    CHECK(inventory.items[2].kind == AMIHEUR_INVENTORY_TASK);
    CHECK(inventory.items[3].kind == AMIHEUR_INVENTORY_RESIDENT);
    CHECK(inventory.items[0].start == 0x10000UL);
    CHECK(inventory.items[0].end == 0x10000UL);

    CHECK(amiheur_amiga_inventory_collect_views(NULL, 1U, &inventory) == -1);
    nodes[0].address = 0UL;
    CHECK(amiheur_amiga_inventory_collect_views(nodes, 1U, &inventory) == -1);

    printf("PASS: Amiga inventory collector core\n");
    return 0;
}
