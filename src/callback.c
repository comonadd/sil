/* File: callback.c */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* SIL callback API and default SIL callbacks */

#include "callback.h"

#include <unistd.h>

#include "types.h"
#include "macros.h"
#include "sil.h"

SILCallbackStatus __handle_enter_key(struct SILState* ss)
{
    if (ss->buffer->len == 0) {
	ss->res = NULL;
	return SILCS_RET_RES;
    }

    /* Allocating the "result" string and copying the */
    /* contents of a curernt buffer to it */
    ss->res = malloc(ss->buffer->len * sizeof(char));
    memcpy(ss->res, ss->buffer->val, ss->buffer->len);
    ss->res[ss->buffer->len] = ZERO_CH;

    sil_refresh(ss);
    write(ss->ofd, "\r\n", 2);
    sil_history_next(ss);
    return SILCS_RET_RES;
}

SILCallbackStatus __handle_tab_key(struct SILState* ss)
{
    uint64 i = 0;
    /* For each "completion from" in the array */
    while (i < ss->completions_count) {
	/* If length of current buffer value is greater than the "completion from" length: */
	if (strcmp(ss->buffer->val, ss->completion_froms[i])) {
	    goto next_completion;
	}
	/* For each character in the buffer value: */
	for (uint64 j = 0; j < ss->buffer->len; ++j) {
	    /* If current buffer character not equals to the current "completion from" character: */
	    if (ss->buffer->val[j] != ss->completion_froms[i][j]) {
		goto next_completion;
	    }
	}
	/* We found it!!! */

	/* Go to the next item in the history */
	sil_history_next(ss);

	/* Copy the value from the array of completions values to the current buffer */
	buf_set(ss->buffer, ss->completion_tos[i], strlen(ss->completion_tos[i]));
	sil_move_cursor_pos_to_end(ss);
	sil_refresh(ss);
	return SILCS_CONTINUE;
    next_completion:
	++i;
    }
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_backspace_key(struct SILState* ss)
{
    if (ss->buffer->len == 0)
	return SILCS_CONTINUE;
    --ss->buffer->len;
    --ss->cursor_pos;
    ss->buffer->val[ss->buffer->len] = ZERO_CH;
    sil_refresh(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_esc_key(struct SILState* ss)
{
    int res;
    char seq[3];
    res = read(ss->ifd, seq, 1);
    if (res == -1) return SILCS_ERROR;
    if (res == 0) return SILCS_CONTINUE;
    res = read(ss->ifd, seq + 1, 1);
    if (res == -1) return SILCS_ERROR;
    if (res == 0) return SILCS_CONTINUE;
    switch (seq[1]) {
	case 'A':;
	    sil_history_prev(ss);
	    sil_refresh(ss);
	    break;
	case 'B':
	    sil_history_next(ss);
	    sil_refresh(ss);
	    break;
	case 'D':
	    sil_move_cursor_pos_left(ss);
	    sil_refresh(ss);
	    break;
	case 'C':
	    sil_move_cursor_pos_right(ss);
	    sil_refresh(ss);
	    break;
	default: PASS();
    }
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_ctrl_L_key(struct SILState* ss)
{
    sil_clear_screen(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_ctrl_C_key(struct SILState* ss)
{
    sil_deinit(ss);
    exit(0);
    return SILCS_CONTINUE;
}

bool sil_key_is_binded(
    struct SILState* ss,
    uint16 key)
{
    if (key >= SIL_MAX_CALLBACKS) return false;
    return ss->config.callbacks[key];
}

bool sil_bind_key(
    struct SILState* ss,
    uint16 key,
    SILCallbackStatus(*callback)(struct SILState*))
{
    if (key >= SIL_MAX_CALLBACKS) return false;
    ss->config.callbacks[key] = callback;
}
