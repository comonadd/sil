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
#include <termios.h>

#include "types.h"
#include "macros.h"
#include "buffer.h"
#include "callback.h"

#define SIL_HISTORY_INITIAL_CAPACITY 32
#define SIL_HISTORY_GROW_RATE 32

enum SILErrno sil_errno = SIL_OK;

static struct termios saved_termios;

static bool init_term(struct SILState* ss);
static bool restore_term(struct SILState* ss);

/************/
/* Terminal */
/************/

/**
   $ Description:
   #   This function initializes the "raw" terminal
**/
static bool init_term(struct SILState* ss)
{
    if (!isatty(STDIN_FILENO)) goto fatal;
    if (tcgetattr(ss->ifd, &saved_termios) == -1) goto fatal;

    struct termios curr_termios;
    curr_termios = saved_termios;

    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    curr_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    /* output modes - disable post processing */
    curr_termios.c_oflag &= ~(OPOST);
    /* control modes - set 8 bit chars */
    curr_termios.c_cflag |= (CS8);
    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    curr_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    /* 1 byte, no timer */
    curr_termios.c_cc[VMIN] = 1; curr_termios.c_cc[VTIME] = 0;

    /* put terminal in raw mode after flushing */
    if (tcsetattr(ss->ifd, TCSAFLUSH, &curr_termios) < 0)
	goto fatal;

    return true;

fatal:
    errno = ENOTTY;
    return false;
}

/**
   $ Description:
   #   This function restores the configuration of a terminal
   #   to the configuration that it had before we changed it
**/
static bool restore_term(struct SILState* ss)
{
    tcsetattr(ss->ifd, TCSAFLUSH, &saved_termios);
    return true;
}

/***********/
/* History */
/***********/

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

NoRet sil_history_next(struct SILState* ss)
{
    if (ss->buffer->len == 0) return;
    if ((ss->history_pos + 1) >= ss->history.size)
	sil_history_grow(&ss->history);
    ++ss->history_pos;
    ss->buffer = ss->history.items[ss->history_pos];
    sil_move_cursor_to_end(ss);
}

NoRet sil_history_prev(struct SILState* ss)
{
    if (ss->history_pos == 0) return;
    --ss->history_pos;
    ss->buffer = ss->history.items[ss->history_pos];
    sil_move_cursor_to_end(ss);
}

/**********/
/* Cursor */
/**********/

NoRet sil_move_cursor_left(struct SILState* ss)
{
    if (ss->cursor_pos == 0) return;
    --ss->cursor_pos;
    sil_refresh(ss);
}

NoRet sil_move_cursor_right(struct SILState* ss)
{
    if (ss->cursor_pos >= ss->buffer->len) return;
    ++ss->cursor_pos;
    sil_refresh(ss);
}

NoRet sil_move_cursor_to_beg(struct SILState* ss)
{
    ss->cursor_pos = 0;
    sil_refresh(ss);
}

NoRet sil_move_cursor_to_end(struct SILState* ss)
{
    ss->cursor_pos = ss->buffer->len;
    sil_refresh(ss);
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
    if (!buf_append(&buf, ss->config.prompt, ss->config.prompt_len))
	return false;

    /* Appending current buffer value */
    if (!buf_append(&buf, ss->buffer->val, ss->buffer->len))
	return false;
    write(ss->ofd, buf.val, buf.len);

    buf_deinit(&buf);
}

bool sil_refresh(struct SILState* ss)
{
    struct Buffer buf;
    if (!buf_init(&buf)) return false;

    /* Appending carriage return character */
    if (!buf_append_ch(&buf, CR_CH)) return false;

    /* Appending prompt */
    if (!buf_append(&buf, ss->config.prompt, ss->config.prompt_len))
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
	ss->config.prompt_len + ss->cursor_pos);
    if (!buf_append(&buf, seq, strlen(seq)))
	return false;
    write(ss->ofd, buf.val, buf.len + 1);

    buf_deinit(&buf);
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
    ss->config.prompt_len = strlen(prompt);
    for (uint16 i = 0; i < SIL_MAX_CALLBACKS; ++i)
	ss->config.callbacks[i] = NULL;
    sil_move_cursor_to_beg(ss);
    ss->ifd = 0;
    ss->ofd = 1;
    if (!init_term(ss)) return false;

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
    restore_term(ss);
}

char* sil_read(struct SILState* ss)
{
    int res;
    uint16 key;
    SILCallbackStatus cs;
    sil_refresh(ss);
    while (true) {
	res = read(ss->ifd, &key, 1);
	if (res == 0) continue;
	if (res == -1) return NULL;
	if (sil_key_is_binded(ss, key)) {
	    cs = ss->config.callbacks[key](ss);
	    switch (cs) {
		case SILCS_CONTINUE: continue;
		case SILCS_RET_RES: return ss->res;
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

static NoRet test_sil_history(void)
{
}

NoRet test_sil(void)
{
    printf("Running tests:\n");
    test_sil_history();
    printf("test_sil_history(): PASSED\n");
}

#endif // TEST
