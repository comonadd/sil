/* File: history.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "history.h"

#include "lib/types.h"
#include "lib/macros.h"
#include "lib/buffer.h"
#include "sil.h"
#include "cursor.h"

#define SIL_HISTORY_INITIAL_CAPACITY 32
#define SIL_HISTORY_GROW_RATE 32

static NoRet sil_history_sync_buf(struct SILState* ss)
{
    ss->buffer = ss->history_items[ss->history_pos];
    sil_move_cursor_pos_to_end(ss);
}

bool sil_history_at_end(struct SILState* ss)
{
    return ss->history_pos == (ss->history_size - 1);
}

bool sil_history_at_beg(struct SILState* ss)
{
    return ss->history_pos == 0;
}

bool sil_history_grow(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");

    const uint64 old_capacity = ss->history_capacity;

    /* Allocating more space to the history */
    ss->history_capacity += SIL_HISTORY_GROW_RATE;
    ss->history_items = realloc(
	ss->history_items,
	ss->history_capacity * PTR_SIZE);
    if (!ss->history_items) return false;

    /* NULLing newly allocated pointers */
    for (uint64 i = (old_capacity - 1); i < ss->history_capacity; ++i)
	ss->history_items[i] = NULL;
    return true;
}

bool sil_history_init(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");

    /* Allocating space for the initial items in the history */
    ss->history_items = malloc(
	SIL_HISTORY_INITIAL_CAPACITY * PTR_SIZE);
    if (!ss->history_items) return false;

    /* NULLing those */
    for (uint64 i = 0; i < SIL_HISTORY_INITIAL_CAPACITY; ++i)
	ss->history_items[i] = NULL;
    ss->history_capacity = SIL_HISTORY_INITIAL_CAPACITY;
    ss->history_size = 0;
    ss->history_pos = 0;
    return true;
}

NoRet sil_history_deinit(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    /* Freeing the each allocated buffer in history */
    for (uint64 i = 0; i < ss->history_size; ++i) {
	buf_deinit(ss->history_items[i]);
	free(ss->history_items[i]);
    }
    /* Freeing the actual array of pointers */
    free(ss->history_items);
}

NoRet sil_history_new_item(struct SILState* ss)
{
    ss->history_items[ss->history_size] = malloc(PTR_SIZE);
    buf_init(ss->history_items[ss->history_size]);
    ++ss->history_size;
}

NoRet sil_history_next(struct SILState* ss)
{
    /* If we have empty buffer, do nothing */
    if (ss->buffer->len == 0) return;

    /* If we at the end of the history, do nothing */
    if (sil_history_at_end(ss)) return;

    ++ss->history_pos;

    /* If we have no space to store 1 more buffer in the history, */
    /* allocate more space */
    if (ss->history_pos >= ss->history_capacity)
	sil_history_grow(ss);

    /* If we buffer at the next position in the history is */
    /* allocated yet, allocate it */
    if (!ss->history_items[ss->history_pos]) {
	ss->history_items[ss->history_pos] = malloc(PTR_SIZE);
	++ss->history_size;
    }

    /* Sync history with the current buffer */
    sil_history_sync_buf(ss);
}

NoRet sil_history_prev(struct SILState* ss)
{
    /* If we have no previous buffers in the history */
    if (sil_history_at_beg(ss)) return;

    /* Decrement current history position and sync */
    /* history with the current buffer */
    --ss->history_pos;
    sil_history_sync_buf(ss);
}

NoRet sil_history_beg(struct SILState* ss)
{
    ss->history_pos = 0;
    sil_history_sync_buf(ss);
}

NoRet sil_history_end(struct SILState* ss)
{
    ss->history_pos = ss->history_size - 1;
    sil_history_sync_buf(ss);
}
