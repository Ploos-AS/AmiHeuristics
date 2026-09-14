#ifndef AMIHEURISTICS_AMIHEURISTICS_H
#define AMIHEURISTICS_AMIHEURISTICS_H

/* Public M0 types. Keep this header usable by conservative C compilers. */

typedef enum AmiHeurDomain {
    AMIHEUR_DOMAIN_FILE = 0,
    AMIHEUR_DOMAIN_BOOT,
    AMIHEUR_DOMAIN_DISK,
    AMIHEUR_DOMAIN_MEMORY
} AmiHeurDomain;

typedef enum AmiHeurSeverity {
    AMIHEUR_SEVERITY_INFO = 0,
    AMIHEUR_SEVERITY_LOW,
    AMIHEUR_SEVERITY_MEDIUM,
    AMIHEUR_SEVERITY_HIGH,
    AMIHEUR_SEVERITY_CRITICAL
} AmiHeurSeverity;

typedef struct AmiHeurFinding {
    const char *rule_id;
    AmiHeurDomain domain;
    AmiHeurSeverity severity;
    int score;
    const char *summary;
    unsigned long offset;
} AmiHeurFinding;

typedef struct AmiHeurScore {
    long total;
    unsigned long finding_count;
} AmiHeurScore;

void amiheur_score_init(AmiHeurScore *score);
int amiheur_score_add(AmiHeurScore *score, const AmiHeurFinding *finding);
const char *amiheur_severity_name(AmiHeurSeverity severity);
const char *amiheur_domain_name(AmiHeurDomain domain);

#endif
