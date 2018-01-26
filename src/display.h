/* File: display.h */
/* Creation Date: 2017-01-11 */
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* SIL display helpers */

#ifndef SIL_DISPLAY_H
#define SIL_DISPLAY_H

#include "lib/types.h"
#include "sil.h"

bool sil_clear_screen(struct SILState* ss);

bool sil_refresh_line(struct SILState* ss);

#endif /* SIL_DISPLAY_H */
