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
    nodes[0].start = 0x0ff00UL; nodes[0].end = 0x10fffUL;
    nodes[0].executable = 1;
    nodes[1].kind = AMIHEUR_INVENTORY_DEVICE;
    nodes[1].name = "trackdisk.device";
    nodes[1].start = 0x1ff00UL; nodes[1].end = 0x20fffUL;
    nodes[1].executable = 1;
    nodes[2].kind = AMIHEUR_INVENTORY_TASK;
    nodes[2].name = "Workbench";
    nodes[2].start = 0x30000UL; nodes[2].end = 0x30000UL;
    nodes[2].executable = 0;
    nodes[3].kind = AMIHEUR_INVENTORY_RESIDENT;
    nodes[3].name = "ramlib";
    nodes[3].start = 0x40000UL; nodes[3].end = 0x40fffUL;
    nodes[3].executable = 1;

    rc = amiheur_amiga_inventory_collect_views(nodes, 4U, &inventory);
    CHECK(rc == 0);
    CHECK(inventory.count == 4U);
    CHECK(strcmp(inventory.items[0].name, "dos.library") == 0);
    CHECK(inventory.items[0].start == 0x0ff00UL);
    CHECK(inventory.items[0].end == 0x10fffUL);
    CHECK(inventory.items[0].executable == 1);
    CHECK(inventory.items[2].kind == AMIHEUR_INVENTORY_TASK);
    CHECK(inventory.items[2].executable == 0);

    CHECK(amiheur_amiga_inventory_collect_views(NULL, 1U, &inventory) == -1);
    nodes[0].start = 0UL;
    CHECK(amiheur_amiga_inventory_collect_views(nodes, 1U, &inventory) == -1);
    nodes[0].start = 0x12000UL; nodes[0].end = 0x11000UL;
    CHECK(amiheur_amiga_inventory_collect_views(nodes, 1U, &inventory) == -1);

    printf("PASS: Amiga inventory explicit range model\n");
    return 0;
}
