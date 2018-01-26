/* File: editing.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "editing.h"

#include "lib/types.h"
#include "lib/macros.h"
#include "sil.h"
#include "cursor.h"
#include "display.h"
#include "history.h"

NoRet sil_insert(
    struct SILState* ss,
    const char ch)
{
    if (sil_cursor_at_end(ss)) {
	/* If we at the end of the buffer, just append char to the buffer */
	buf_append_ch(ss->buffer, ch);
    } else {
	/* If not, allocate space for one more character, */
	/* shift all characters after the current cursor position */
	/* to the right by one, and then, just insert the character to the */
	/* current cursor position */
	buf_append_ch(ss->buffer, ZERO_CH);
	for (uint64 i = ss->buffer->len; i > ss->cursor_pos; --i)
	    ss->buffer->val[i] = ss->buffer->val[i - 1];
	ss->buffer->val[ss->cursor_pos] = ch;
    }
    sil_move_cursor_pos_right(ss);
    sil_refresh_line(ss);
}

NoRet sil_delete_next_char(struct SILState* ss)
{
    if (ss->cursor_pos == ss->buffer->len) {
	/* If we got nothing after the current cursor */
	/* position, return */
	return;
    }

    /* Shifting the each character starting at the current cursor position */
    /* to the left by one */
    for (uint64 i = ss->cursor_pos; i < ss->buffer->len; ++i)
	ss->buffer->val[i] = ss->buffer->val[i + 1];
    --ss->buffer->len;
    ss->buffer->val[ss->buffer->len] = ZERO_CH;
}

NoRet sil_delete_prev_char(struct SILState* ss)
{
    /* If we got nothing in the buffer or we at the */
    /* beginning of the buffer, return */
    if ((ss->cursor_pos == 0) || (ss->buffer->len == 0))
	return;

    /* If we at the end of the buffer, just erase */
    /* the last character in the buffer */
    if (ss->cursor_pos == ss->buffer->len) {
	--ss->buffer->len;
	ss->buffer->val[ss->buffer->len] = ZERO_CH;
	sil_move_cursor_pos_left(ss);
	return;
    }

    /* Shifting the each character after the current cursor position */
    /* to the left by one */
    for (uint64 i = ss->cursor_pos; i < ss->buffer->len; ++i)
	ss->buffer->val[i - 1] = ss->buffer->val[i];
    --ss->buffer->len;
    ss->buffer->val[ss->buffer->len] = ZERO_CH;
    sil_move_cursor_pos_left(ss);
}

NoRet sil_delete_prev_word(struct SILState* ss)
{
    /* If we got nothing in the buffer or we at the */
    /* beginning of the buffer, return */
    if ((ss->cursor_pos == 0) || (ss->buffer->len == 0))
	return;

    uint64 old_pos = ss->cursor_pos;
    uint64 old_len = ss->buffer->len;
    uint64 pos = ss->cursor_pos;
    if (ss->buffer->val[pos - 1] == SPACE_CH) {
	while ((pos != 0) && (ss->buffer->val[pos - 1] == SPACE_CH)) {
	    --pos;
	    --ss->buffer->len;
	    --ss->cursor_pos;
	}
    } else {
	while ((pos != 0) && (ss->buffer->val[pos - 1] != SPACE_CH)) {
	    --pos;
	    --ss->buffer->len;
	    --ss->cursor_pos;
	}
    }

    memmove(&ss->buffer->val[pos], &ss->buffer->val[old_pos], old_len - old_pos);
}

NoRet sil_complete(struct SILState* ss)
{
    uint64 i = 0;
    /* For each "completion from" in the array */
    while (i < ss->completions_count) {
	/* If length of current buffer value is greater than the */
	/* current "completion from" length: */
	if (strcmp(ss->buffer->val, ss->completion_froms[i]))
	    goto next_completion;
	/* For each character in the buffer value: */
	for (uint64 j = 0; j < ss->buffer->len; ++j)
	    /* If current buffer character not equals to the current "completion from" character: */
	    if (ss->buffer->val[j] != ss->completion_froms[i][j])
		goto next_completion;
	/* We found it!!! */

	/* Go to the next item in the history */
	sil_history_next(ss);

	/* Copy the value from the array of completions values to the current buffer */
	buf_set(ss->buffer, ss->completion_tos[i], strlen(ss->completion_tos[i]));
	sil_move_cursor_pos_to_end(ss);
	sil_refresh_line(ss);
	return;
    next_completion:
	++i;
    }
}
