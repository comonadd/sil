/* File: callback.h */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* SIL callback API and default SIL callbacks */

#ifndef CALLBACK_H
#define CALLBACK_H

#include "sil.h"

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

bool sil_key_is_binded(
    struct SILState* ss,
    uint16 key);
bool sil_bind_key(
    struct SILState* ss,
    uint16 key,
    SILCallbackStatus(*callback)(struct SILState*));

/*********************/
/* Default callbacks */
/*********************/

SILCallbackStatus __handle_enter_key(struct SILState* ss);
SILCallbackStatus __handle_tab_key(struct SILState* ss);
SILCallbackStatus __handle_backspace_key(struct SILState* ss);
SILCallbackStatus __handle_esc_key(struct SILState* ss);
SILCallbackStatus __handle_ctrl_L_key(struct SILState* ss);
SILCallbackStatus __handle_ctrl_C_key(struct SILState* ss);

#endif // CALLBACK_H
