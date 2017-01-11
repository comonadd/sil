/* File: history.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "history.h"

#include "types.h"
#include "buffer.h"
#include "macros.h"

#define SIL_HISTORY_INITIAL_CAPACITY 32
#define SIL_HISTORY_GROW_RATE 32

bool sil_history_grow(struct SILHistory* sh)
{
    DEBUG_ASSERT(sh, "");
    uint16 old_size = sh->size;
    sh->size += SIL_HISTORY_GROW_RATE;
    sh->items = realloc(sh->items, sh->size * sizeof(struct Buffer));
    if (!sh->items) return false;
    for (uint16 i = (old_size - 1); i < sh->size; ++i) {
	sh->items[i] = malloc(sizeof(struct Buffer));
	buf_init(sh->items[i]);
    }
    return true;
}

bool sil_history_init(struct SILHistory* sh)
{
    DEBUG_ASSERT(sh, "");
    sh->size = SIL_HISTORY_INITIAL_CAPACITY;
    sh->items = malloc(SIL_HISTORY_INITIAL_CAPACITY * sizeof(struct Buffer));
    if (!sh->items) return false;
    for (uint16 i = 0; i < SIL_HISTORY_INITIAL_CAPACITY; ++i) {
	sh->items[i] = malloc(sizeof(struct Buffer));
	buf_init(sh->items[i]);
    }
    sh->size = 0;
    return true;
}

NoRet sil_history_deinit(struct SILHistory* sh)
{
    DEBUG_ASSERT(sh, "");
    for (uint16 i = 0; i < sh->size; ++i) {
	buf_deinit(sh->items[i]);
	free(sh->items[i]);
    }
    free(sh->items);
}
