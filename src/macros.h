/* File: macros.h */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */

#ifndef MACROS_H
#define MACROS_H

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#define ZERO_CH '\0'
#define CR_CH '\r'
#define LF_CH '\n'
#define CRLF "\r\n"

#define CLEAR_SCREEN_SEQ "\x1b[H\x1b[2J"
#define CLEAR_SCREEN_SEQ_LEN 7

#define ERASE_RIGHT_SEQ "\x1b[0K"
#define ERASE_RIGHT_SEQ_LEN 4

#define PASS()
#define CAST(a, type) (type)a
#define UNUSED(a) CAST(a, void)
#define STREQ(a, b) (strcmp(a, b) == 0)
#define STREQ_CI(a, b) (strcasecmp(a, b) == 0)

#if DEBUG
#  define UNLIKELY() assert(0)
#  define DEBUG_ASSERT(cond, msg) do {					\
	if (!(cond)) {							\
	    printf("[%s:%d] Assertion failed (%s): %s\n", __FILE__, __LINE__, #cond, msg); \
	    exit(0);\
	}					\
    } while (0);
#  define DEBUG_ASSERT_IF(cond, cond_if, msg) do {			\
	if ((cond_if) && !(cond)) {					\
	    printf("[%s:%d] Assertion failed ((%s) && (%s)): %s\n", __FILE__, __LINE__, #cond_if, #cond, msg); \
	    exit(0);\
	}								\
    } while (0)
#else
#  define DEBUG_ASSERT(cond, msg)
#  define DEBUG_ASSERT_IF(cond, cond_if, msg)
#endif // DEBUG

#endif // MACROS_H
