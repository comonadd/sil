/* File: sil.h */
/* Creation Date: 2017-01-10 */
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef SIL_H
#define SIL_H

#include "lib/types.h"
#include "lib/buffer.h"

#define SIL_MAX_CALLBACKS 256

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

struct SILState;

typedef SILCallbackStatus (*SILCallback)(struct SILState*);

/**
   $ Description:
   @config - configuration
   @history_size - size of a history
   @completions_count - count of completions
   @completions_size - count of completions that we allocated memory for
   @buffer - current buffer in history
   @history_items - array that stores history items
   @completion_froms - array of strings to complete from
   @completion_tos - array of strings to complete to
   @res - thing that callbacks should change if they want to,
   #      and it will be returned if callback said "SILCS_RET_RES!"
   @ifd - terminal input file descriptor
   @ofd - terminal output file descriptor
   @cursor_pos - current cursor position in the buffer
   @history_pos - current position in the history
**/
struct SILState {
    struct{
	char const* prompt;
	SILCallback callbacks[SIL_MAX_CALLBACKS];
    } config;
    uint64 history_size;
    uint64 history_capacity;
    uint64 completions_count;
    uint64 completions_size;
    struct Buffer* buffer;
    struct Buffer** history_items;
    char const** completion_froms;
    char const** completion_tos;
    char* res;
    uint32 prompt_len;
    uint32 prompt_actual_len;
    int ifd;
    int ofd;
    uint16 cursor_pos;
    uint16 history_pos;
};

extern enum SILErrno sil_errno;

/***************/
/* Completions */
/***************/

bool sil_add_completion(
    struct SILState* ss,
    const char* complete_from,
    const char* complete_to);

/********/
/* Main */
/********/

/**
   $ Description:
   #   This function initializes the Simple Input Library structure
   $ Return value:
   #   Returns true if succeeds, false otherwise
**/
bool sil_init(
    struct SILState* ss,
    const char* prompt);

/**
   $ Description:
   #   This function initializes the Simple Input Library structure
   $ Return value:
   #   Returns true if succeeds, false otherwise
**/
bool sil_deinit(struct SILState* ss);

/**
   $ Description:
   #   This function reads a line from the user
   $ Input:
   #   nothing
   $ Output:
   #   line that user has entered
   $ Return value:
   #   Returns line if succeeds, NULL otherwise
**/
char* sil_read(struct SILState* ss);

#if TEST
NoRet test_sil(void);
#endif /* TEST */

#endif /* SIL_H */
