/* File: cursor.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "cursor.h"

#include "lib/types.h"
#include "lib/macros.h"
#include "sil.h"
#include "display.h"

bool sil_cursor_at_beg(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    return ss->cursor_pos == 0;
}

bool sil_cursor_at_end(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    DEBUG_ASSERT(ss->buffer, "");
    return ss->cursor_pos == ss->buffer->len;
}

NoRet sil_move_cursor_pos_left(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    if (sil_cursor_at_beg(ss)) return;
    --ss->cursor_pos;
}

NoRet sil_move_cursor_pos_left_by_word(struct SILState* ss)
{
    if (sil_cursor_at_beg(ss)) return;
    if (ss->buffer->val[ss->cursor_pos - 1] == SPACE_CH) {
	while ((ss->cursor_pos != 0) &&
	       (ss->buffer->val[ss->cursor_pos - 1] == SPACE_CH))
	    --ss->cursor_pos;
    } else {
	while ((ss->cursor_pos != 0)
	       && (ss->buffer->val[ss->cursor_pos - 1] != SPACE_CH))
	    --ss->cursor_pos;
    }
}

NoRet sil_move_cursor_pos_right(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    if (sil_cursor_at_end(ss)) return;
    ++ss->cursor_pos;
}

NoRet sil_move_cursor_pos_to_beg(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    ss->cursor_pos = 0;
}

NoRet sil_move_cursor_pos_to_end(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    if (sil_cursor_at_end(ss)) return;
    ss->cursor_pos = ss->buffer->len;
}
