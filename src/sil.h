/* File: sil.h */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef SIL_H
#define SIL_H

#include <limits.h>

#include "types.h"
#include "buffer.h"

#define SIL_HISTORY_INITIAL_CAPACITY 32
#define SIL_HISTORY_GROW_RATE 32
#define SIL_MAX_CALLBACKS 256

/**
   $ Description:
   #   This enumeration represents a key codes that SHOULD
   #   be used in order to bind a callback
**/
enum KeyCode {
    KC_CTRL_C = 3,
    KC_CTRL_L = 12,
    KC_ENTER = 13,
    KC_ESC = 27,
    KC_BACKSPACE = 127,
    KC_TAB = 9
};

/**
   $ Description:
   #   This enumeration represents return value of a callback
   #   and says "what to do" after callback execution
   @SILCS_CONTINUE - should continue ask for input
   @SILCS_ERROR - should report about error and return NULL
   @SILCS_RET_RES - should "res" field of a SIL state
**/
typedef enum SILCallbackStatus {
    SILCS_CONTINUE,
    SILCS_ERROR,
    SILCS_RET_RES
} SILCallbackStatus;

enum SILErrno {
    SIL_OK,
    SIL_BAD
};

/**
   @items - items of history
   @size - count of items that we allocated buffers for
**/
struct SILHistory {
    struct Buffer** items;
    uint16 size;
};

/**
   $ Description:
   @buffer - current buffer in history
   @config - configuration
   @history - history of a input
   @config - configuration
   @ifd - terminal input file descriptor
   @ofd - terminal output file descriptor
   @cursor_pos - current cursor position in the buffer
   @history_pos - current position in the history
   @res - thing that callbacks should change if they want to,
	  this thing will be returned if callback said "SILCS_RET_RES!"
**/
struct SILState {
    struct Buffer* buffer;
    struct {
	char const* prompt;
	SILCallbackStatus (*callbacks[SIL_MAX_CALLBACKS])(struct SILState*);
    } config;
    struct SILHistory history;
    uint64 completions_count;
    uint64 completions_size;
    char* res;
    char const** completion_tos;
    char const** completion_froms;
    int ifd;
    int ofd;
    uint16 cursor_pos;
    uint16 history_pos;
    uint8 prompt_len;
};

extern enum SILErrno sil_errno;

/***********/
/* History */
/***********/

/**
   $ Description:
   #   This function grows the array of items of the history
   $ Return value:
   #   Returns true if succeeds
   #   Returns false only if there is no enough memory
**/
bool sil_history_grow(struct SILHistory* sh);

/**
   $ Description:
   #   This function initializes history and
   #   initializes all the buffers in it
   $ Return value:
   #   Returns true if succeeds
   #   Returns false if there is not enough memory
**/
bool sil_history_init(struct SILHistory* sh);

/**
   $ Description:
   #   This function deinitializes the history,
   #   and frees all the buffers in it
   $ Return value:
   #   Always succeeds, so there is no reasons to return
   #   anything
**/
NoRet sil_history_deinit(struct SILHistory* sh);

/**
   $ Description:
**/
NoRet sil_history_next(struct SILState* ss);

/**
   $ Description:
**/
NoRet sil_history_prev(struct SILState* ss);

/***************/
/* Completions */
/***************/

bool sil_add_completion(
    struct SILState* ss,
    const char const* complete_from,
    const char const* complete_to);

/**********/
/* Cursor */
/**********/

/**
   $ Description:
   #   This function just moves cursor to the left
   $ Return value:
   #   This function always succeeds
**/
NoRet sil_move_cursor_pos_left(struct SILState* ss);

/**
   $ Description:
   #   This function just moves cursor to the right
   $ Return value:
   #   This function always succeeds
**/
NoRet sil_move_cursor_pos_right(struct SILState* ss);

/**
   $ Description:
   #   This function just moves cursor to the beginning of the buffer
   $ Return value:
   #   This function always succeeds
**/
NoRet sil_move_cursor_pos_to_beg(struct SILState* ss);

/**
   $ Description:
   #   This function just moves cursor to the end of the buffer
   $ Return value:
   #   This function always succeeds
**/
NoRet sil_move_cursor_pos_to_end(struct SILState* ss);

/********/
/* Main */
/********/

/**
   $ Description:
   #   This function clears the screen
   $ Return value:
   #   Always succeeds
**/
bool sil_clear_screen(struct SILState* ss);

/**
   $ Description:
   #   This function refreshes the current buffer
   $ Return value:
   #   Always succeeds
**/
bool sil_refresh(struct SILState* ss);

/**
   $ Description:
   #   This function initializes the Simple Input Library structure
   $ Return value:
   #   Returns true if succeeds, false otherwise
**/
bool sil_init(
    struct SILState* ss,
    const char const* prompt);

/**
   $ Description:
   #   This function initializes the Simple Input Library structure
   $ Return value:
   #   Returns true if succeeds, false otherwise
**/
bool sil_deinit(struct SILState* ss);

/**
   $ Description:
   #   This function initializes the Simple Input Library structure
   $ Input:
   #   nothing
   $ Output:
   #   line that user has entered
   $ Return value:
   #   Returns true if succeeds, false otherwise
**/
char* sil_read(struct SILState* ss);

#if TEST
NoRet test_sil(void);
#endif // TEST

#endif // SIL_H
