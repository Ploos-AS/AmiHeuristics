#include "amiheuristics/exec_lvo.h"

#include <stddef.h>

static const AmiHeurExecLvoInfo g_exec_lvos[] = {
    { -102, "InitResident", AMIHEUR_EXEC_LVO_MUTATION },
    { -120, "Disable", AMIHEUR_EXEC_LVO_CONTROL },
    { -126, "Enable", AMIHEUR_EXEC_LVO_CONTROL },
    { -132, "Forbid", AMIHEUR_EXEC_LVO_CONTROL },
    { -138, "Permit", AMIHEUR_EXEC_LVO_CONTROL },
    { -162, "SetIntVector", AMIHEUR_EXEC_LVO_MUTATION },
    { -168, "AddIntServer", AMIHEUR_EXEC_LVO_MUTATION },
    { -174, "RemIntServer", AMIHEUR_EXEC_LVO_MUTATION },
    { -198, "AllocMem", AMIHEUR_EXEC_LVO_NORMAL },
    { -210, "FreeMem", AMIHEUR_EXEC_LVO_NORMAL },
    { -282, "AddTask", AMIHEUR_EXEC_LVO_MUTATION },
    { -288, "RemTask", AMIHEUR_EXEC_LVO_MUTATION },
    { -396, "AddLibrary", AMIHEUR_EXEC_LVO_MUTATION },
    { -402, "RemLibrary", AMIHEUR_EXEC_LVO_MUTATION },
    { -414, "CloseLibrary", AMIHEUR_EXEC_LVO_NORMAL },
    { -420, "SetFunction", AMIHEUR_EXEC_LVO_MUTATION },
    { -432, "AddDevice", AMIHEUR_EXEC_LVO_MUTATION },
    { -438, "RemDevice", AMIHEUR_EXEC_LVO_MUTATION },
    { -444, "OpenDevice", AMIHEUR_EXEC_LVO_NORMAL },
    { -450, "CloseDevice", AMIHEUR_EXEC_LVO_NORMAL },
    { -456, "DoIO", AMIHEUR_EXEC_LVO_NORMAL },
    { -462, "SendIO", AMIHEUR_EXEC_LVO_NORMAL },
    { -480, "AbortIO", AMIHEUR_EXEC_LVO_NORMAL },
    { -486, "AddResource", AMIHEUR_EXEC_LVO_MUTATION },
    { -492, "RemResource", AMIHEUR_EXEC_LVO_MUTATION },
    { -552, "OpenLibrary", AMIHEUR_EXEC_LVO_NORMAL }
};

int amiheur_exec_lvo_lookup(int offset, AmiHeurExecLvoInfo *info)
{
    size_t i;

    if (info == 0)
        return -1;

    for (i = 0U; i < sizeof(g_exec_lvos) / sizeof(g_exec_lvos[0]); ++i) {
        if (g_exec_lvos[i].offset == offset) {
            *info = g_exec_lvos[i];
            return 1;
        }
    }

    info->offset = offset;
    info->name = "Unknown";
    info->classification = AMIHEUR_EXEC_LVO_UNKNOWN;
    return 0;
}
