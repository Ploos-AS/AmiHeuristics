#ifndef AMIHEURISTICS_EXEC_LVO_H
#define AMIHEURISTICS_EXEC_LVO_H

typedef enum AmiHeurExecLvoClass {
    AMIHEUR_EXEC_LVO_UNKNOWN = 0,
    AMIHEUR_EXEC_LVO_NORMAL,
    AMIHEUR_EXEC_LVO_CONTROL,
    AMIHEUR_EXEC_LVO_MUTATION
} AmiHeurExecLvoClass;

typedef struct AmiHeurExecLvoInfo {
    int offset;
    const char *name;
    AmiHeurExecLvoClass classification;
} AmiHeurExecLvoInfo;

int amiheur_exec_lvo_lookup(int offset, AmiHeurExecLvoInfo *info);

#endif
