/* File: terminal.h */
/* Creation Date: 2017-01-11 */
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* Helpers for working with terminal */

#ifndef TERMINAL_H
#define TERMINAL_H

#include "lib/types.h"

/**
   $ Description:
   #   This function returns true if
   #   current terminal supported by us
   $ Return value:
   #   Returns true if supported, false otherwise
**/
bool term_is_supported(void);

/**
   $ Description:
   #   This function initializes the "raw" terminal
**/
bool term_enable_raw_mode(const int fd);

/**
   $ Description:
   #   This function restores the configuration of a terminal
   #   to the configuration that it had before we changed it
**/
bool term_disable_raw_mode(const int fd);

#endif /* TERMINAL_H */
