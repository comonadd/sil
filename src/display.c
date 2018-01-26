/* File: display.c */
/* Creation Date: 2017-01-11 */
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* SIL display helpers */

#include "display.h"

#include <unistd.h>

#include "lib/types.h"
#include "lib/macros.h"
#include "sil.h"

bool sil_clear_screen(struct SILState* ss)
{
    struct Buffer buf;
    buf_init(&buf);

    /* Appending special escape sequence to clear the screen */
    buf_append(&buf, CLEAR_SCREEN_SEQ, CLEAR_SCREEN_SEQ_LEN);

    /* Appending prompt */
    buf_append(&buf, ss->config.prompt, ss->prompt_len);

    /* Appending current buffer value */
    buf_append(&buf, ss->buffer->val, ss->buffer->len);

    /* Actually refreshing the display */
    if (write(ss->ofd, buf.val, buf.len) == -1) {
	PASS();
    }

    buf_deinit(&buf);
    return true;
}

bool sil_refresh_line(struct SILState* ss)
{
    struct Buffer buf;
    buf_init(&buf);

    /* Appending CR (carriage return) character */
    buf_append_ch(&buf, CR_CH);

    /* Appending prompt */
    buf_append(&buf, ss->config.prompt, ss->prompt_len);

    /* Appending current buffer value */
    buf_append(&buf, ss->buffer->val, ss->buffer->len);

    /* Appending escape sequence to erase all stuff in the right */
    buf_append(&buf, ERASE_RIGHT_SEQ, ERASE_RIGHT_SEQ_LEN);

    /* Appending escape sequence to set the current cursor position */
    char seq[64];
    snprintf(
	seq, 64,
	"\r\x1b[%dC",
	ss->prompt_actual_len + ss->cursor_pos);
    buf_append(&buf, seq, strlen(seq));
    if (write(ss->ofd, buf.val, buf.len) == -1) {
	PASS();
    }

    buf_deinit(&buf);
    return true;
}
