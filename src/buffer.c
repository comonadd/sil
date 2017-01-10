/* File: buffer.c */
/* Creation Date: 2017-01-10*/
/* Creator: Dmitry Guzeev <dmitry.guzeev@yahoo.com> */
/* Description: */
/* Generic buffer implementation */

#include "buffer.h"

#include <stdlib.h>

#include "types.h"
#include "macros.h"

static bool buf_grow(struct Buffer* sb)
{
    sb->capacity += BUF_GROW_RATE;
    sb->val = realloc(sb->val, sb->capacity * sizeof(char));
    if (!sb->val) return false;
    return true;
}

bool buf_init(struct Buffer* sb)
{
    sb->capacity = BUF_INITIAL_CAPACITY;
    sb->val = malloc(BUF_INITIAL_CAPACITY * sizeof(char));
    if (!sb->val) return false;
    sb->val[0] = ZERO_CH;
    sb->len = 0;
    return true;
}

NoRet buf_deinit(struct Buffer* sb)
{
    free(sb->val);
}

NoRet buf_clear(struct Buffer* sb)
{
    sb->val[0] = ZERO_CH;
    sb->len = 0;
}

bool buf_append_ch(
    struct Buffer* sb,
    char ch)
{
    ++sb->len;
    if (sb->len >= sb->capacity)
	if (!buf_grow(sb)) return false;
    sb->val[sb->len - 1] = ch;
    sb->val[sb->len] = ZERO_CH;
    return true;
}

bool buf_append(
    struct Buffer* sb,
    const char const* str,
    const uint64 len)
{
    while ((sb->len + len) >= sb->capacity)
	if (!buf_grow(sb)) return false;
    memcpy(&sb->val[sb->len], str, len);
    sb->len += len;
    sb->val[sb->len] = ZERO_CH;
    return true;
}

bool buf_set(
    struct Buffer* sb,
    const char const* str,
    const uint64 len)
{
    sb->len = len;
    if (sb->len >= sb->capacity) {
	free(sb->val);
	sb->val = malloc(sb->capacity * sizeof(char));
	if (!sb->val) return false;
    }
    memcpy(sb->val, str, len);
    sb->val[len] = ZERO_CH;
    return true;
}
