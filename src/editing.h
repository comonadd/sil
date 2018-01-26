/* File: editing.h */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef EDITING_H
#define EDITING_H

#include "lib/types.h"
#include "sil.h"

NoRet sil_insert(
    struct SILState* ss,
    const char ch);

NoRet sil_delete_next_char(struct SILState* ss);

NoRet sil_delete_prev_char(struct SILState* ss);

NoRet sil_delete_prev_word(struct SILState* ss);

NoRet sil_complete(struct SILState* ss);

#endif // EDITING_H
