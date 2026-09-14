#include "amiheuristics/hunk.h"
#include "amiheuristics/m68k.h"
#include <string.h>

#define HUNK_HEADER 0x3f3UL
#define HUNK_CODE 0x3e9UL
#define HUNK_DATA 0x3eaUL
#define HUNK_BSS 0x3ebUL
#define HUNK_RELOC32 0x3ecUL
#define HUNK_RELOC16 0x3edUL
#define HUNK_RELOC8 0x3eeUL
#define HUNK_END 0x3f2UL

static unsigned long be32(const unsigned char *p)
{
    return ((unsigned long)p[0] << 24) | ((unsigned long)p[1] << 16) |
           ((unsigned long)p[2] << 8) | (unsigned long)p[3];
}

static int get32(const unsigned char *data, size_t size, size_t *pos,
                 unsigned long *value)
{
    if (*pos > size || size - *pos < 4U)
        return -1;
    *value = be32(data + *pos);
    *pos += 4U;
    return 0;
}

static int skip_words(const unsigned char *data, size_t size, size_t *pos,
                      unsigned long count)
{
    size_t bytes;
    (void)data;
    if (count > (unsigned long)(size / 4U))
        return -1;
    bytes = (size_t)count * 4U;
    if (*pos > size || size - *pos < bytes)
        return -1;
    *pos += bytes;
    return 0;
}

static void finding(AmiHeurHunkReport *r, const char *id, int weight)
{
    if (r->finding_count < AMIHEUR_HUNK_MAX_FINDINGS)
        r->finding_ids[r->finding_count] = id;
    ++r->finding_count;
    r->score += weight;
}

static int inspect_code(const unsigned char *data, size_t size)
{
    size_t i;
    AmiHeurM68kInsn insn;
    for (i = 0U; i + 1U < size; i += 2U) {
        if (amiheur_m68k_decode(data + i, size - i, &insn) == 0 &&
            insn.is_a6_lvo && insn.lvo_offset < 0)
            return 1;
    }
    return 0;
}

static int skip_reloc(const unsigned char *data, size_t size, size_t *pos)
{
    unsigned long count;
    unsigned long target;
    unsigned long i;
    for (;;) {
        if (get32(data, size, pos, &count) != 0)
            return -1;
        if (count == 0UL)
            return 0;
        if (get32(data, size, pos, &target) != 0)
            return -1;
        for (i = 0UL; i < count; ++i)
            if (get32(data, size, pos, &target) != 0)
                return -1;
    }
}

int amiheur_hunk_analyze(const unsigned char *data, size_t size,
                         AmiHeurHunkReport *report)
{
    size_t pos;
    unsigned long word;
    unsigned long count;
    unsigned long first;
    unsigned long last;
    unsigned long i;
    unsigned long segs;
    unsigned long words;

    if (data == 0 || report == 0)
        return -1;
    memset(report, 0, sizeof(*report));
    if (size < 16U)
        return -1;
    pos = 0U;
    if (get32(data, size, &pos, &word) != 0 || word != HUNK_HEADER)
        return 0;
    for (;;) {
        if (get32(data, size, &pos, &word) != 0)
            return 0;
        if (word == 0UL)
            break;
        if (skip_words(data, size, &pos, word) != 0)
            return 0;
    }
    if (get32(data, size, &pos, &count) != 0 ||
        get32(data, size, &pos, &first) != 0 ||
        get32(data, size, &pos, &last) != 0)
        return 0;
    if (count == 0UL || last < first || last - first + 1UL != count || count > 1024UL)
        return 0;
    for (i = 0UL; i < count; ++i)
        if (get32(data, size, &pos, &words) != 0)
            return 0;
    report->hunk_count = count;
    segs = 0UL;
    while (pos < size && segs < count) {
        if (get32(data, size, &pos, &word) != 0)
            return 0;
        if (word == HUNK_CODE || word == HUNK_DATA || word == HUNK_BSS) {
            if (get32(data, size, &pos, &words) != 0 ||
                words > (unsigned long)(size / 4U))
                return 0;
            if (pos > size || (size_t)words > (size - pos) / 4U)
                return 0;
            if (word == HUNK_CODE) {
                report->has_code = 1;
                report->code_bytes += words * 4UL;
                if (inspect_code(data + pos, (size_t)words * 4U))
                    report->has_a6_lvo_call = 1;
            } else if (word == HUNK_DATA) {
                report->has_data = 1;
                report->data_bytes += words * 4UL;
            } else {
                report->has_bss = 1;
                report->bss_bytes += words * 4UL;
            }
            pos += (size_t)words * 4U;
        } else if (word == HUNK_RELOC32 || word == HUNK_RELOC16 || word == HUNK_RELOC8) {
            if (skip_reloc(data, size, &pos) != 0)
                return 0;
            ++report->relocation_hunks;
        } else if (word == HUNK_END) {
            ++segs;
        } else {
            return 0;
        }
    }
    if (segs != count)
        return 0;
    report->valid = 1;
    finding(report, "HUNK.VALID_HEADER", 0);
    if (report->has_code)
        finding(report, "HUNK.CODE", 0);
    if (report->has_data)
        finding(report, "HUNK.DATA", 0);
    if (report->has_bss)
        finding(report, "HUNK.BSS", 0);
    if (report->relocation_hunks != 0UL)
        finding(report, "HUNK.RELOCATIONS", 0);
    if (report->has_a6_lvo_call)
        finding(report, "HUNK.A6_LVO_CALL", 10);
    return 0;
}
