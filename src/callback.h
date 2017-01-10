/* File: callback.h */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef CALLBACK_H
#define CALLBACK_H

#include "sil.h"

/*********************/
/* Default callbacks */
/*********************/

SILCallbackStatus __handle_enter_key(struct SILState* ss);
SILCallbackStatus __handle_tab_key(struct SILState* ss);
SILCallbackStatus __handle_backspace_key(struct SILState* ss);
SILCallbackStatus __handle_esc_key(struct SILState* ss);
SILCallbackStatus __handle_ctrl_L_key(struct SILState* ss);
SILCallbackStatus __handle_ctrl_C_key(struct SILState* ss);
bool sil_key_is_binded(
    struct SILState* ss,
    uint16 key);

#endif // CALLBACK_H
