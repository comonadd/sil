/* File: terminal.c */
/* Creation Date: 2017-01-11*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* Helpers for working with terminal */

#include "terminal.h"

#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#include "lib/types.h"
#include "lib/macros.h"

static char* unsupported_terminals[] = {"emacs", "dumb", NULL};
static struct termios saved_termios;

bool term_is_supported(void)
{
    char* term = getenv("TERM");
    if (!term) return 0;
    for (uint8 i = 0; unsupported_terminals[i]; ++i)
	if (STREQ_CI(term, unsupported_terminals[i]))
	    return false;
    return true;
}

bool term_enable_raw_mode(const int fd)
{
    if (!isatty(STDIN_FILENO)) goto fatal;
    if (tcgetattr(fd, &saved_termios) == -1)
	goto fatal;

    struct termios curr_termios;
    curr_termios = saved_termios;

    /* input modes: no break, no CR to NL, no parity check, no strip char,
     * no start/stop output control. */
    curr_termios.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    /* output modes - disable post processing */
    curr_termios.c_oflag &= ~(OPOST);

    /* control modes - set 8 bit chars */
    curr_termios.c_cflag |= (CS8);

    /* local modes - choing off, canonical off, no extended functions,
     * no signal chars (^Z,^C) */
    curr_termios.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    /* control chars - set return condition: min number of bytes and timer.
     * We want read to return every single byte, without timeout. */
    /* 1 byte, no timer */
    curr_termios.c_cc[VMIN] = 1; curr_termios.c_cc[VTIME] = 0;

    /* put terminal in raw mode after flushing */
    if (tcsetattr(fd, TCSAFLUSH, &curr_termios) == -1)
	goto fatal;

    return true;

fatal:
    errno = ENOTTY;
    return false;
}

bool term_disable_raw_mode(const int fd)
{
    tcsetattr(fd, TCSAFLUSH, &saved_termios);
    return true;
}
