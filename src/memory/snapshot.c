#include <string.h>
#include "amiheuristics/snapshot.h"

static int valid_view(const AmiHeurSnapshotView *view)
{
    size_t i;
    if (view == NULL) return 0;
    if (view->item_count != 0U && view->items == NULL) return 0;
    if (view->vector_count != 0U && view->vectors == NULL) return 0;
    for (i = 0U; i < view->item_count; ++i) {
        if (view->items[i].name == NULL || view->items[i].start > view->items[i].end)
            return 0;
    }
    for (i = 0U; i < view->vector_count; ++i) {
        if (view->vectors[i].name == NULL) return 0;
    }
    return 1;
}

static long find_item(const AmiHeurSnapshotView *view,
                      AmiHeurInventoryKind kind, const char *name)
{
    size_t i;
    for (i = 0U; i < view->item_count; ++i) {
        if (view->items[i].kind == kind && strcmp(view->items[i].name, name) == 0)
            return (long)i;
    }
    return -1L;
}

static long find_vector(const AmiHeurSnapshotView *view, const char *name)
{
    size_t i;
    for (i = 0U; i < view->vector_count; ++i) {
        if (strcmp(view->vectors[i].name, name) == 0) return (long)i;
    }
    return -1L;
}

static void emit(AmiHeurDiffEvent *events, size_t capacity, size_t index,
                 AmiHeurDiffKind kind, const char *name,
                 AmiHeurInventoryKind inventory_kind,
                 unsigned long old_start, unsigned long old_end,
                 unsigned long new_start, unsigned long new_end,
                 unsigned long old_target, unsigned long new_target)
{
    AmiHeurDiffEvent *event;
    if (events == NULL || index >= capacity) return;
    event = &events[index];
    event->kind = kind;
    event->name = name;
    event->inventory_kind = inventory_kind;
    event->old_start = old_start;
    event->old_end = old_end;
    event->new_start = new_start;
    event->new_end = new_end;
    event->old_target = old_target;
    event->new_target = new_target;
}

int amiheur_snapshot_diff(const AmiHeurSnapshotView *before,
                          const AmiHeurSnapshotView *after,
                          AmiHeurDiffEvent *events,
                          size_t capacity,
                          size_t *required_count)
{
    size_t i;
    size_t count = 0U;
    long match;

    if (required_count == NULL || !valid_view(before) || !valid_view(after))
        return -1;
    if (capacity != 0U && events == NULL) return -1;

    for (i = 0U; i < before->item_count; ++i) {
        match = find_item(after, before->items[i].kind, before->items[i].name);
        if (match < 0L) {
            emit(events, capacity, count, AMIHEUR_DIFF_INVENTORY_REMOVED,
                 before->items[i].name, before->items[i].kind,
                 before->items[i].start, before->items[i].end, 0UL, 0UL, 0UL, 0UL);
            ++count;
        } else if (before->items[i].start != after->items[match].start ||
                   before->items[i].end != after->items[match].end ||
                   before->items[i].executable != after->items[match].executable ||
                   before->items[i].allow_patch != after->items[match].allow_patch) {
            emit(events, capacity, count, AMIHEUR_DIFF_INVENTORY_CHANGED,
                 before->items[i].name, before->items[i].kind,
                 before->items[i].start, before->items[i].end,
                 after->items[match].start, after->items[match].end, 0UL, 0UL);
            ++count;
        }
    }
    for (i = 0U; i < after->item_count; ++i) {
        if (find_item(before, after->items[i].kind, after->items[i].name) < 0L) {
            emit(events, capacity, count, AMIHEUR_DIFF_INVENTORY_ADDED,
                 after->items[i].name, after->items[i].kind,
                 0UL, 0UL, after->items[i].start, after->items[i].end, 0UL, 0UL);
            ++count;
        }
    }

    for (i = 0U; i < before->vector_count; ++i) {
        match = find_vector(after, before->vectors[i].name);
        if (match < 0L) {
            emit(events, capacity, count, AMIHEUR_DIFF_VECTOR_REMOVED,
                 before->vectors[i].name, AMIHEUR_INVENTORY_LIBRARY,
                 0UL, 0UL, 0UL, 0UL, before->vectors[i].target, 0UL);
            ++count;
        } else if (before->vectors[i].target != after->vectors[match].target) {
            emit(events, capacity, count, AMIHEUR_DIFF_VECTOR_RETARGETED,
                 before->vectors[i].name, AMIHEUR_INVENTORY_LIBRARY,
                 0UL, 0UL, 0UL, 0UL,
                 before->vectors[i].target, after->vectors[match].target);
            ++count;
        }
    }
    for (i = 0U; i < after->vector_count; ++i) {
        if (find_vector(before, after->vectors[i].name) < 0L) {
            emit(events, capacity, count, AMIHEUR_DIFF_VECTOR_ADDED,
                 after->vectors[i].name, AMIHEUR_INVENTORY_LIBRARY,
                 0UL, 0UL, 0UL, 0UL, 0UL, after->vectors[i].target);
            ++count;
        }
    }

    *required_count = count;
    if (count > capacity) return -2;
    return 0;
}
