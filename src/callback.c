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
    if (ss->buffer->len == 0)
	return SILCS_CONTINUE;
    ss->res = malloc(ss->buffer->len * sizeof(char));
    memcpy(ss->res, ss->buffer->val, ss->buffer->len);
    ss->res[ss->buffer->len] = ZERO_CH;
    sil_refresh(ss);
    write(ss->ofd, CRLF, 2);
    sil_history_next(ss);
    return SILCS_RET_RES;
}

/* TODO: Completion */
SILCallbackStatus __handle_tab_key(struct SILState* ss)
{
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
	    break;
	case 'B':
	    sil_history_next(ss);
	    break;
	case 'D':
	    sil_move_cursor_left(ss);
	    break;
	case 'C':
	    sil_move_cursor_right(ss);
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
