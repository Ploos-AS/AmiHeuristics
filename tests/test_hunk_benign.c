#include "amiheuristics/hunk.h"
#include <stdio.h>

#define BENIGN_MAX_SCORE 20

static void put32(unsigned char *p, unsigned long v)
{
    p[0] = (unsigned char)((v >> 24) & 0xffUL);
    p[1] = (unsigned char)((v >> 16) & 0xffUL);
    p[2] = (unsigned char)((v >> 8) & 0xffUL);
    p[3] = (unsigned char)(v & 0xffUL);
}

static size_t hunk_prefix(unsigned char *b, unsigned long code_words)
{
    size_t p = 0U;
    put32(b + p, 0x3f3UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 1UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, 0UL); p += 4U;
    put32(b + p, code_words); p += 4U;
    put32(b + p, 0x3e9UL); p += 4U;
    put32(b + p, code_words); p += 4U;
    return p;
}

static size_t finish(unsigned char *b, size_t p)
{
    put32(b + p, 0x3f2UL);
    return p + 4U;
}

static size_t make_plain_program(unsigned char *b)
{
    size_t p = hunk_prefix(b, 1UL);
    b[p++] = 0x4eU; b[p++] = 0x71U; /* NOP */
    b[p++] = 0x4eU; b[p++] = 0x75U; /* RTS */
    return finish(b, p);
}

static size_t make_normal_exec_program(unsigned char *b)
{
    size_t p = hunk_prefix(b, 3UL);
    b[p++] = 0x2cU; b[p++] = 0x78U; b[p++] = 0x00U; b[p++] = 0x04U;
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xffU; b[p++] = 0x3aU; /* AllocMem -198 */
    b[p++] = 0x4eU; b[p++] = 0x75U;
    b[p++] = 0x4eU; b[p++] = 0x71U;
    return finish(b, p);
}

static size_t make_control_pair(unsigned char *b)
{
    size_t p = hunk_prefix(b, 3UL);
    b[p++] = 0x2cU; b[p++] = 0x78U; b[p++] = 0x00U; b[p++] = 0x04U;
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xffU; b[p++] = 0x88U; /* Disable -120 */
    b[p++] = 0x4eU; b[p++] = 0xaeU; b[p++] = 0xffU; b[p++] = 0x82U; /* Enable -126 */
    return finish(b, p);
}

static size_t make_unpacker_like(unsigned char *b)
{
    size_t p = hunk_prefix(b, 2UL);
    b[p++] = 0x0aU; b[p++] = 0x18U; b[p++] = 0x00U; b[p++] = 0x55U; /* EORI.B #$55,(A0)+ */
    b[p++] = 0x60U; b[p++] = 0xfaU; /* BRA.S backwards */
    b[p++] = 0x4eU; b[p++] = 0x71U;
    return finish(b, p);
}

static int expect_benign(const char *name, size_t (*make)(unsigned char *))
{
    unsigned char b[128];
    AmiHeurHunkReport r;
    size_t n;

    n = make(b);
    if (amiheur_hunk_analyze(b, n, &r) != 0 || !r.valid) {
        printf("FAIL: benign corpus parse: %s\n", name);
        return 1;
    }
    if (r.score > BENIGN_MAX_SCORE) {
        printf("FAIL: benign corpus score: %s=%d\n", name, r.score);
        return 1;
    }
    return 0;
}

int main(void)
{
    if (expect_benign("plain", make_plain_program) != 0)
        return 1;
    if (expect_benign("normal-exec", make_normal_exec_program) != 0)
        return 1;
    if (expect_benign("control-pair", make_control_pair) != 0)
        return 1;
    if (expect_benign("unpacker-like", make_unpacker_like) != 0)
        return 1;

    puts("PASS: benign HUNK corpus stays below false-positive threshold");
    return 0;
}
