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

#include "lib/types.h"
#include "lib/macros.h"
#include "lib/buffer.h"
#include "callback.h"
#include "terminal.h"
#include "editing.h"
#include "cursor.h"
#include "display.h"
#include "history.h"

enum SILErrno sil_errno = SIL_OK;

/**********************/
/* Private prototypes */
/**********************/

static uint64 __calc_len_of_esc_str(
    const char* str,
    const uint64 len);

/***********/
/* Helpers */
/***********/

static uint64 __calc_len_of_esc_str(
    const char* str,
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

/***************/
/* Completions */
/***************/

bool sil_add_completion(
    struct SILState* ss,
    const char* complete_from,
    const char* complete_to)
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

/********/
/* Main */
/********/

bool sil_init(
    struct SILState* ss,
    const char* prompt)
{
    if (!sil_history_init(ss)) return false;

    ss->config.prompt = prompt;
    ss->prompt_len = strlen(prompt);
    ss->prompt_actual_len = __calc_len_of_esc_str(prompt, ss->prompt_len);

    for (uint16 i = 0; i < SIL_MAX_CALLBACKS; ++i)
	ss->config.callbacks[i] = NULL;

    sil_move_cursor_pos_to_beg(ss);
    ss->ifd = 0;
    ss->ofd = 1;

    ss->completions_size = 64;
    ss->completions_count = 0;
    ss->completion_froms = malloc(
	ss->completions_size * sizeof(*ss->completion_froms));
    ss->completion_tos = malloc(
	ss->completions_size * sizeof(*ss->completion_tos));

    sil_bind_key(ss, KC_ENTER, __handle_enter_key);
    sil_bind_key(ss, KC_TAB, __handle_tab_key);
    sil_bind_key(ss, KC_BACKSPACE, __handle_backspace_key);
    sil_bind_key(ss, KC_DEL, __handle_del_key);
    sil_bind_key(ss, KC_ESC, __handle_esc_key);
    sil_bind_key(ss, KC_CTRL_L, __handle_ctrl_L_key);
    sil_bind_key(ss, KC_CTRL_C, __handle_ctrl_C_key);
    sil_bind_key(ss, KC_CTRL_T, __handle_ctrl_H_key);
    sil_bind_key(ss, KC_CTRL_BACKSPACE, __handle_ctrl_backspace_key);

    return true;
}

bool sil_deinit(struct SILState* ss)
{
    sil_history_deinit(ss);
    return true;
}

char* sil_read(struct SILState* ss)
{
    int res;
    uint16 key;
    SILCallbackStatus cs;

    if (!isatty(ss->ifd)) {

    } else if (term_is_supported()) {
	sil_history_new_item(ss);
	sil_history_end(ss);
	ss->cursor_pos = 0;
	term_enable_raw_mode(ss->ifd);
	sil_refresh_line(ss);
	while (true) {
	    res = read(ss->ifd, &key, 1);
	    if (res == 0) continue;
	    if (res == -1) return NULL;

	    /* If the given key is in the bindings list */
	    if (sil_key_is_binded(ss, key)) {
		/* Call the callback that key binded to */
		cs = ss->config.callbacks[key](ss);
		/* Do the stuff that callback said */
		switch (cs) {
		    case SILCS_CONTINUE: continue;
		    case SILCS_RET_RES:
			term_disable_raw_mode(ss->ifd);
			write(ss->ofd, "\n", 1);
			return ss->res;
		    case SILCS_ERROR: return NULL;
		    default: UNLIKELY();
		}
	    } else {
		/* If key is not binded */
		/* Just print the actual character */
		sil_insert(ss, key);
	    }
	}
    } else {
	res = write(ss->ofd, ss->config.prompt, ss->prompt_len);
	if ((res == 0) || (res == -1)) return NULL;
	fflush(stdout);

	static const uint64 max_len = 4096;
	char* line = malloc(max_len * sizeof(char));
	if (!fgets(line, max_len, stdin)) {
	    free(line);
	    return NULL;
	}

	uint64 len = strlen(line);
	while ((len != 0) &&
	       (line[len - 1] == CR_CH || line[len - 1] == LF_CH)) {
	    line[--len] = ZERO_CH;
	}

	return line;
    }

    return NULL;
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

#endif /* TEST */
