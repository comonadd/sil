/* File: history.h */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef HISTORY_H
#define HISTORY_H

#include "types.h"
#include "buffer.h"

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

#endif // HISTORY_H
