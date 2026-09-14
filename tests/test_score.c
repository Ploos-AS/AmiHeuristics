#include <stdio.h>
#include <string.h>
#include "amiheuristics/amiheuristics.h"

static int fail(const char *message)
{
    fprintf(stderr, "FAIL: %s\n", message);
    return 1;
}

int main(void)
{
    AmiHeurScore score;
    AmiHeurFinding finding;

    amiheur_score_init(&score);
    if (score.total != 0 || score.finding_count != 0) {
        return fail("score init");
    }

    finding.rule_id = "M0.TEST";
    finding.domain = AMIHEUR_DOMAIN_BOOT;
    finding.severity = AMIHEUR_SEVERITY_MEDIUM;
    finding.score = 25;
    finding.summary = "test finding";
    finding.offset = 0;

    if (amiheur_score_add(&score, &finding) != 0) {
        return fail("score add return code");
    }
    if (score.total != 25 || score.finding_count != 1) {
        return fail("score add values");
    }
    if (strcmp(amiheur_domain_name(finding.domain), "boot") != 0) {
        return fail("domain name");
    }
    if (strcmp(amiheur_severity_name(finding.severity), "MEDIUM") != 0) {
        return fail("severity name");
    }
    if (amiheur_score_add(0, &finding) == 0 || amiheur_score_add(&score, 0) == 0) {
        return fail("null rejection");
    }

    puts("PASS: M0 scoring core");
    return 0;
}
