#include "amiheuristics/amiheuristics.h"

void amiheur_score_init(AmiHeurScore *score)
{
    if (score == 0) {
        return;
    }
    score->total = 0;
    score->finding_count = 0;
}

int amiheur_score_add(AmiHeurScore *score, const AmiHeurFinding *finding)
{
    if (score == 0 || finding == 0) {
        return -1;
    }

    score->total += (long)finding->score;
    score->finding_count++;
    return 0;
}

const char *amiheur_severity_name(AmiHeurSeverity severity)
{
    switch (severity) {
    case AMIHEUR_SEVERITY_INFO: return "INFO";
    case AMIHEUR_SEVERITY_LOW: return "LOW";
    case AMIHEUR_SEVERITY_MEDIUM: return "MEDIUM";
    case AMIHEUR_SEVERITY_HIGH: return "HIGH";
    case AMIHEUR_SEVERITY_CRITICAL: return "CRITICAL";
    default: return "UNKNOWN";
    }
}

const char *amiheur_domain_name(AmiHeurDomain domain)
{
    switch (domain) {
    case AMIHEUR_DOMAIN_FILE: return "file";
    case AMIHEUR_DOMAIN_BOOT: return "boot";
    case AMIHEUR_DOMAIN_DISK: return "disk";
    case AMIHEUR_DOMAIN_MEMORY: return "memory";
    default: return "unknown";
    }
}
