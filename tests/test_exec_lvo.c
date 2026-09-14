#include "amiheuristics/exec_lvo.h"

#include <stdio.h>
#include <string.h>

static int expect_lvo(int offset, const char *name, AmiHeurExecLvoClass cls)
{
    AmiHeurExecLvoInfo info;

    if (amiheur_exec_lvo_lookup(offset, &info) != 1)
        return 1;
    if (info.offset != offset || info.classification != cls)
        return 1;
    if (strcmp(info.name, name) != 0)
        return 1;
    return 0;
}

int main(void)
{
    AmiHeurExecLvoInfo info;

    if (expect_lvo(-420, "SetFunction", AMIHEUR_EXEC_LVO_MUTATION) != 0)
        return 1;
    if (expect_lvo(-168, "AddIntServer", AMIHEUR_EXEC_LVO_MUTATION) != 0)
        return 1;
    if (expect_lvo(-132, "Forbid", AMIHEUR_EXEC_LVO_CONTROL) != 0)
        return 1;
    if (expect_lvo(-198, "AllocMem", AMIHEUR_EXEC_LVO_NORMAL) != 0)
        return 1;
    if (expect_lvo(-552, "OpenLibrary", AMIHEUR_EXEC_LVO_NORMAL) != 0)
        return 1;
    if (amiheur_exec_lvo_lookup(-999, &info) != 0)
        return 1;
    if (info.classification != AMIHEUR_EXEC_LVO_UNKNOWN)
        return 1;
    if (amiheur_exec_lvo_lookup(-420, 0) != -1)
        return 1;

    puts("PASS: Exec LVO classifier");
    return 0;
}
