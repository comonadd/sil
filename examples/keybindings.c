/* File: keybindings.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* This example shows how to bind keys to callbacks */

#include "sil.h"
#include "callback.h"

#include <stdio.h>

SILCallbackStatus do_whatever(struct SILState* ss)
{
    printf("helloooo\n");
    return SILCS_CONTINUE;
}

int main(void)
{
    struct SILState ss;
    sil_init(&ss, "$> ");

    sil_bind_key(&ss, KC_CTRL_L, do_whatever);

    char* line;
    do {
	line = sil_read(&ss);
	printf("Echo: %s\n", line);
    } while (sil_errno == SIL_OK);

    return 0;
}
