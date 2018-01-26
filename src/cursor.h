/* File: cursor.h */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef SIL_CURSOR_H
#define SIL_CURSOR_H

#include "lib/types.h"
#include "sil.h"

bool sil_cursor_at_beg(struct SILState* ss);

bool sil_cursor_at_end(struct SILState* ss);

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

#endif // SIL_CURSOR_H
