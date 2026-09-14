#include "amiheuristics/hunk.h"
#include <stdio.h>
#include <string.h>

static void put32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)((v >> 24) & 0xffUL);
    p[1] = (unsigned char)((v >> 16) & 0xffUL);
    p[2] = (unsigned char)((v >> 8) & 0xffUL);
    p[3] = (unsigned char)(v & 0xffUL);
}

static int finding(const AmiHeurHunkReport *r, const char *id)
{
    unsigned long i;
    for (i = 0UL; i < r->finding_count && i < AMIHEUR_HUNK_MAX_FINDINGS; ++i)
        if (strcmp(r->finding_ids[i], id) == 0)
            return 1;
    return 0;
}

static size_t make_simple(unsigned char *b)
{
    size_t p = 0U;
    put32(b + p, 0x3f3UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 1UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 0x3e9UL); p += 4U;
    put32(b + p, 1UL); p += 4U;
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xfeU; b[p++] = 0x5cU;
    put32(b + p, 0x3f2UL); p += 4U;
    return p;
}

static size_t make_control_mutation(unsigned char *b)
{
    size_t p = 0U;
    put32(b + p, 0x3f3UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 1UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 2UL); p += 4U;
    put32(b + p, 0x3e9UL); p += 4U;
    put32(b + p, 2UL); p += 4U;
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xffU; b[p++] = 0x7cU;
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xfeU; b[p++] = 0x5cU;
    put32(b + p, 0x3f2UL); p += 4U;
    return p;
}

static int test_valid(void)
{
    unsigned char b[128];
    AmiHeurHunkReport r;
    size_t n;
    n = make_simple(b);
    if (amiheur_hunk_analyze(b, n, &r) != 0)
        return 1;
    if (!r.valid || r.hunk_count != 1UL || r.code_bytes != 4UL)
        return 1;
    if (!r.has_code || !r.has_a6_lvo_call || !finding(&r, "HUNK.A6_LVO_CALL"))
        return 1;
    if (!r.has_exec_mutation_lvo || r.exec_mutation_lvo_count != 1UL)
        return 1;
    if (!finding(&r, "HUNK.EXEC_MUTATION_LVO"))
        return 1;
    return r.score != 30;
}

static int test_control_mutation(void)
{
    unsigned char b[128];
    AmiHeurHunkReport r;
    size_t n;
    n = make_control_mutation(b);
    if (amiheur_hunk_analyze(b, n, &r) != 0 || !r.valid)
        return 1;
    if (!r.has_exec_control_lvo || !r.has_exec_mutation_lvo)
        return 1;
    if (r.exec_control_lvo_count != 1UL || r.exec_mutation_lvo_count != 1UL)
        return 1;
    if (!finding(&r, "HUNK.EXEC_CONTROL_LVO") ||
        !finding(&r, "HUNK.EXEC_MUTATION_LVO") ||
        !finding(&r, "HUNK.CORR_EXEC_CONTROL_MUTATION"))
        return 1;
    return r.score != 45;
}

static int test_truncated(void)
{
    unsigned char b[128];
    AmiHeurHunkReport r;
    size_t n;
    n = make_simple(b);
    if (n < 2U)
        return 1;
    if (amiheur_hunk_analyze(b, n - 2U, &r) != 0)
        return 1;
    return r.valid != 0;
}

static int test_bad_magic(void)
{
    unsigned char b[16];
    AmiHeurHunkReport r;
    memset(b, 0, sizeof(b));
    if (amiheur_hunk_analyze(b, sizeof(b), &r) != 0)
        return 1;
    return r.valid != 0;
}

int main(void)
{
    if (test_valid() != 0) { puts("FAIL: valid HUNK"); return 1; }
    if (test_control_mutation() != 0) { puts("FAIL: Exec semantic HUNK"); return 1; }
    if (test_truncated() != 0) { puts("FAIL: truncated HUNK"); return 1; }
    if (test_bad_magic() != 0) { puts("FAIL: bad HUNK magic"); return 1; }
    puts("PASS: HUNK parser and Exec semantic file heuristics");
    return 0;
}
