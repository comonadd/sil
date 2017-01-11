/* File: sil.c */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "sil.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "types.h"
#include "macros.h"
#include "buffer.h"
#include "callback.h"
#include "terminal.h"

enum SILErrno sil_errno = SIL_OK;

/*******************/
/* History helpers */
/*******************/

NoRet sil_history_next(struct SILState* ss)
{
    if (ss->buffer->len == 0) return;
    if ((ss->history_pos + 1) >= ss->history.size)
	sil_history_grow(&ss->history);
    ++ss->history_pos;
    ss->buffer = ss->history.items[ss->history_pos];
    sil_move_cursor_pos_to_end(ss);
}

NoRet sil_history_prev(struct SILState* ss)
{
    if (ss->history_pos == 0) return;
    --ss->history_pos;
    ss->buffer = ss->history.items[ss->history_pos];
    sil_move_cursor_pos_to_end(ss);
}

/***************/
/* Completions */
/***************/

bool sil_add_completion(
    struct SILState* ss,
    const char const* complete_from,
    const char const* complete_to)
{
    if (ss->completions_count >= ss->completions_size) {
	ss->completions_size += 64;
	ss->completion_froms = realloc(ss->completion_froms, ss->completions_size);
	ss->completion_tos = realloc(ss->completion_tos, ss->completions_size);
    }
    ss->completion_froms[ss->completions_count] = complete_from;
    ss->completion_tos[ss->completions_count] = complete_to;
    ++ss->completions_count;
    return true;
}

/**********/
/* Cursor */
/**********/

NoRet sil_move_cursor_pos_left(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    if (ss->cursor_pos == 0) return;
    --ss->cursor_pos;
}

NoRet sil_move_cursor_pos_right(struct SILState* ss)
{
    DEBUG_ASSERT(ss, "");
    if (ss->cursor_pos >= ss->buffer->len) return;
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
    ss->cursor_pos = ss->buffer->len;
}

/********/
/* Main */
/********/

bool sil_clear_screen(struct SILState* ss)
{
    struct Buffer buf;
    if (!buf_init(&buf)) return false;

    /* Appending special escape sequence to clear the screen */
    if (!buf_append(&buf, CLEAR_SCREEN_SEQ, CLEAR_SCREEN_SEQ_LEN))
	return false;

    /* Appending prompt */
    if (!buf_append(&buf, ss->config.prompt, ss->prompt_len))
	return false;

    /* Appending current buffer value */
    if (!buf_append(&buf, ss->buffer->val, ss->buffer->len))
	return false;
    write(ss->ofd, buf.val, buf.len);

    buf_deinit(&buf);
}

bool sil_refresh_line(struct SILState* ss)
{
    /* Initializing the temporary buffer */
    struct Buffer buf;
    if (!buf_init(&buf)) return false;

    /* Appending carriage return character */
    if (!buf_append_ch(&buf, CR_CH)) return false;

    /* Appending prompt */
    if (!buf_append(&buf, ss->config.prompt, ss->prompt_len))
	return false;

    /* Appending current buffer value */
    if (!buf_append(&buf, ss->buffer->val, ss->buffer->len))
	return false;

    /* Appending escape sequence to erase all stuff in the right */
    if (!buf_append(&buf, ERASE_RIGHT_SEQ, ERASE_RIGHT_SEQ_LEN))
	return false;

    /* Appending escape sequence to set the current cursor position */
    char seq[64];
    snprintf(
	seq, 64,
	"\r\x1b[%dC",
	ss->prompt_actual_len + ss->cursor_pos);
    if (!buf_append(&buf, seq, strlen(seq)))
	return false;
    write(ss->ofd, buf.val, buf.len);

    /* Deinitializing the temporary buffer */
    buf_deinit(&buf);
}

static uint64 __calc_len_of_esc_str(
    const char const* str,
    const uint64 len)
{
    uint64 res = 0;
    for (uint64 i = 0; i < len; ++i) {
	if (str[i] == '\033') {
	    while (str[i] != 'm') ++i;
	    continue;
	}
	++res;
    }
    return res;
}

bool sil_init(
    struct SILState* ss,
    const char const* prompt)
{
    if (!sil_history_init(&ss->history))
	return false;
    ss->history_pos = 0;
    ss->buffer = ss->history.items[0];
    ss->config.prompt = prompt;

    ss->prompt_len = strlen(prompt);
    ss->prompt_actual_len = __calc_len_of_esc_str(prompt, ss->prompt_len);

    for (uint16 i = 0; i < SIL_MAX_CALLBACKS; ++i)
	ss->config.callbacks[i] = NULL;
    sil_move_cursor_pos_to_beg(ss);
    ss->ifd = 0;
    ss->ofd = 1;
    if (!term_init(ss->ifd)) return false;

    ss->completions_size = 64;
    ss->completions_count = 0;
    ss->completion_froms = malloc(ss->completions_size * sizeof(char*));
    ss->completion_tos = malloc(ss->completions_size * sizeof(char*));

    sil_bind_key(ss, KC_ENTER, __handle_enter_key);
    sil_bind_key(ss, KC_TAB, __handle_tab_key);
    sil_bind_key(ss, KC_BACKSPACE, __handle_backspace_key);
    sil_bind_key(ss, KC_ESC, __handle_esc_key);
    sil_bind_key(ss, KC_CTRL_L, __handle_ctrl_L_key);
    sil_bind_key(ss, KC_CTRL_C, __handle_ctrl_C_key);

    return true;
}

bool sil_deinit(struct SILState* ss)
{
    sil_history_deinit(&ss->history);
    term_restore(ss->ifd);
}

char* sil_read(struct SILState* ss)
{
    int res;
    uint16 key;
    SILCallbackStatus cs;
    sil_refresh_line(ss);
    while (true) {
	res = read(ss->ifd, &key, 1);
	if (res == 0) continue;
	if (res == -1) return NULL;
	if (sil_key_is_binded(ss, key)) {
	    write(ss->ofd, "\r", 1);
	    cs = ss->config.callbacks[key](ss);
	    sil_refresh_line(ss);
	    switch (cs) {
		case SILCS_CONTINUE: continue;
		case SILCS_RET_RES:
		    write(ss->ofd, "\r", 1);
		    return ss->res;
		case SILCS_ERROR: return NULL;
		default: UNLIKELY();
	    }
	} else {
	    buf_append_ch(ss->buffer, key);
	    write(ss->ofd, &key, 1);
	    ++ss->cursor_pos;
	}
    }
    UNLIKELY();
}

/**********/
/* Tests */
/**********/

#if TEST

#include <assert.h>

static NoRet test_calc_len_of_esc_str(void)
{
    char* a = "\033[38;5;226mhello\033[0m";
    assert(__calc_len_of_esc_str(a, strlen(a)) == 5);

    char* b = "\033[38;5;226mmoooo\033[0m";
    assert(__calc_len_of_esc_str(a, strlen(b)) == 5);
}

NoRet test_sil(void)
{
    printf("Running tests:\n");
    test_calc_len_of_esc_str();
    printf("test_calc_len_of_esc_str(): PASSED\n");
}

#endif // TEST
