/* File: callback.c */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* SIL callback API and default SIL callbacks */

#include "callback.h"

#include <unistd.h>

#include "lib/types.h"
#include "lib/macros.h"
#include "sil.h"
#include "cursor.h"
#include "editing.h"
#include "display.h"
#include "history.h"
#include "terminal.h"

/********/
/* Main */
/********/

bool sil_key_is_binded(
    struct SILState* ss,
    const uint16 key)
{
    if (key >= SIL_MAX_CALLBACKS) return false;
    return ss->config.callbacks[key];
}

bool sil_bind_key(
    struct SILState* ss,
    const uint16 key,
    SILCallback callback)
{
    if (key >= SIL_MAX_CALLBACKS) return false;
    ss->config.callbacks[key] = callback;
    return true;
}

/*********************/
/* Default callbacks */
/*********************/

SILCallbackStatus __handle_enter_key(struct SILState* ss)
{
    /* If there is nothing in the buffer */
    if (ss->buffer->len == 0) {
	/* All is ok, but we return NULL to the caller */
	ss->res = NULL;
	return SILCS_RET_RES;
    }

    /* Allocating the "result" string and copying the */
    /* contents of a curernt buffer to it */
    ss->res = malloc(ss->buffer->len * sizeof(char));
    memcpy(ss->res, ss->buffer->val, ss->buffer->len);
    ss->res[ss->buffer->len] = ZERO_CH;

    int res = write(ss->ofd, "\r", 1);
    if ((res == -1) || (res == 0))
	return SILCS_ERROR;
    return SILCS_RET_RES;
}

SILCallbackStatus __handle_tab_key(struct SILState* ss)
{
    sil_complete(ss);
    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_backspace_key(struct SILState* ss)
{
    sil_delete_prev_char(ss);
    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_del_key(struct SILState* ss)
{
    sil_delete_next_char(ss);
    sil_refresh_line(ss);
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
	case 'A':
	    /* UP Arrow */
	    sil_history_prev(ss);
	    break;
	case 'B':
	    /* DOWN Arrow */
	    sil_history_next(ss);
	    break;
	case 'D':
	    /* LEFT Arrow */
	    sil_move_cursor_pos_left(ss);
	    break;
	case 'C':
	    /* RIGHT Arrow */
	    sil_move_cursor_pos_right(ss);
	    break;
	case '1':
	    if (seq[2] == ';') {
		sil_move_cursor_pos_right(ss);
	    }
	    break;
	case '3':
	    /* DEL */
	    /* while (ss->buffer->val[ss->cursor_pos] == ' ') { */
	    /*	sil_delete_next_char(ss); */
	    /* } */
	    break;
	    /* __handle_ctrl_del_key(ss); */
	case '7':
	    /* HOME */
	    sil_move_cursor_pos_to_beg(ss);
	    break;
	case '8':
	    /* END */
	    sil_move_cursor_pos_to_end(ss);
	    break;
	default: return SILCS_CONTINUE;
    }

    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_ctrl_backspace_key(struct SILState* ss)
{
    sil_delete_prev_word(ss);
    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}

/* SILCallbackStatus __handle_ctrl_del_key(struct SILState* ss) */
/* { */
/*     return SILCS_CONTINUE; */
/* } */

SILCallbackStatus __handle_ctrl_L_key(struct SILState* ss)
{
    sil_clear_screen(ss);
    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_ctrl_C_key(struct SILState* ss)
{
    if (write(ss->ofd, CRLF, 2) == -1)
	return SILCS_ERROR;
    sil_deinit(ss);
    term_disable_raw_mode(ss->ifd);
    exit(0);
    return SILCS_CONTINUE;
}

SILCallbackStatus __handle_ctrl_H_key(struct SILState* ss)
{
    write(ss->ofd, CRLF, 2);
    uint64 p = ss->history_size - 1;
    for (uint64 i = 0; i < ss->history_size; ++i) {
	if (write(ss->ofd, ss->history_items[i]->val, ss->history_items[i]->len) == -1) {
	    return SILCS_ERROR;
	}
	if (i != p) write(ss->ofd, CRLF, 2);
    }
    sil_refresh_line(ss);
    return SILCS_CONTINUE;
}
