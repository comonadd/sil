/* File: main.c */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#include <stdlib.h>
#include <stdio.h>

#include "sil.h"
#include "macros.h"

int main(int argc, char** argv)
{
    if (argc > 1) {
	uint16 i = 1;
	while (argv[i]) {
	    if (argv[i][0] != '-') continue;
	    switch (argv[i][1]) {
#if TEST
		case 't':
		    test_sil();
		    exit(0);
#endif // TEST
		default: PASS();
	    }
	    ++i;
	}
    }

    struct SILState ss;
    sil_init(&ss, ">> ");
    char* line;
    do {
	line = sil_read(&ss);
	printf("Echo: %s\n", line);
	free(line);
    } while (sil_errno == SIL_OK);

    sil_deinit(&ss);
    return 0;
}
