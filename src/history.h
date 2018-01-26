/* File: history.h */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef SIL_HISTORY_H
#define SIL_HISTORY_H

#include "lib/types.h"
#include "lib/buffer.h"
#include "sil.h"

bool sil_history_at_end(struct SILState* ss);
bool sil_history_at_beg(struct SILState* ss);
bool sil_history_grow(struct SILState* ss);
bool sil_history_init(struct SILState* ss);
NoRet sil_history_deinit(struct SILState* ss);
NoRet sil_history_new_item(struct SILState* ss);
NoRet sil_history_next(struct SILState* ss);
NoRet sil_history_prev(struct SILState* ss);
NoRet sil_history_beg(struct SILState* ss);
NoRet sil_history_end(struct SILState* ss);

#endif /* SIL_HISTORY_H */
