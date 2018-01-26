/* File: example_basic.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include "sil.h"

#include <stdio.h>

int main(void)
{
    struct SILState ss;
    sil_init(&ss, ">>> ");
    char* line;
    do {
	line = sil_read(&ss);
	if (line) {
	    /* printf("%s\n", line); */
	}
    } while (sil_errno == SIL_OK);
    sil_deinit(&ss);
    return 0;
}
