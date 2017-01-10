/* File: buffer.h */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* Generic buffer implementation */

#ifndef BUFFER_H
#define BUFFER_H

#include "types.h"

#define BUF_INITIAL_CAPACITY 1024
#define BUF_GROW_RATE 1024

struct Buffer {
    uint64 len;
    uint64 capacity;
    char* val;
};

bool buf_init(struct Buffer* sb);

NoRet buf_deinit(struct Buffer* sb);

NoRet buf_clear(struct Buffer* sb);

bool buf_append_ch(
    struct Buffer* sb,
    char ch);

bool buf_append(
    struct Buffer* sb,
    const char const* str,
    const uint64 len);

bool buf_set(
    struct Buffer* sb,
    const char const* str,
    const uint64 len);

#endif // BUFFER_H
