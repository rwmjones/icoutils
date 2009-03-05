/* strbuf.c - The string buffer data-structure.
 *
 * Copyright (C) 1998-2005 Oskar Liljeblad
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include <sys/types.h>		/* POSIX */
#include <stdarg.h>		/* C89 */
#include <stdlib.h>		/* C89 */
#include <string.h>		/* C89 - for memmove */
#include <stdio.h>		/* C89 */
#include "vasprintf.h"		/* Gnulib */
#include "xalloc.h"		/* Gnulib */
#include "minmax.h"		/* Gnulib */
#include "strbuf.h"		/* common */

const int DEFAULT_CAPACITY = 16;

typedef struct {
	uint32_t capacity;
	uint32_t count;
	char value[0];
} StringBuffer;

static inline StringBuffer *
get_string_buffer(char **buf)
{
	return (StringBuffer *) (*buf - sizeof(StringBuffer));
}

static inline char *
new_strbuf(uint32_t capacity, uint32_t count)
{
	StringBuffer *strbuf;
	strbuf = xmalloc(sizeof(StringBuffer) + sizeof(char)*capacity);
	strbuf->capacity = capacity;
	strbuf->count = count;
	return strbuf->value;
}

char *
strbuf_new(void)
{
	char *buf;
	buf = new_strbuf(DEFAULT_CAPACITY, 0);
	buf[0] = '\0';
	return buf;
}

char *
strbuf_new_with_capacity(uint32_t capacity)
{
	char *buf;
	buf = new_strbuf(capacity, 0);
	buf[0] = '\0';
	return buf;
}

char *
strbuf_new_from_string(const char *string)
{
	return strbuf_new_from_substring(string, 0, strlen(string));
}

char *
strbuf_to_string(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	char *string;

	string = xmalloc(sizeof(char) * (strbuf->count + 1));
	memcpy(string, buf, strbuf->count + 1);

	return string;
}

char *
strbuf_substring(char **buf, uint32_t begin_index, uint32_t end_index)
{
	char *string;
	int32_t len = end_index - begin_index;

	if (len < 0)
		return NULL;

	string = xmalloc(sizeof(char) * (len + 1));
	memcpy(string, buf+begin_index, len);
	string[len] = '\0';

	return string;
}

char *
strbuf_substring_to_end(char **buf, uint32_t begin_index)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	return strbuf_substring(buf, begin_index, strbuf->count);
}

void
strbuf_free(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	free(strbuf);
}

static inline void
ensure_capacity(char **buf, StringBuffer **strbuf, uint32_t min_capacity)
{
	if (min_capacity > (*strbuf)->capacity) {
		uint32_t max = (min_capacity > (*strbuf)->count ? (*strbuf)->count*2+2 : (*strbuf)->count);
		min_capacity = MAX(min_capacity, max);

		*strbuf = xrealloc(*strbuf, sizeof(StringBuffer) + sizeof(char) * min_capacity);
		(*strbuf)->capacity = min_capacity;
		*buf = (*strbuf)->value;
	}
}

void
strbuf_ensure_capacity(char **buf, uint32_t min_capacity)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	ensure_capacity(buf, &strbuf, min_capacity);
}

void
strbuf_set_length(char **buf, uint32_t new_length)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	ensure_capacity(buf, &strbuf, new_length + 1);
	//memclear(*buf + strbuf->count, new_length - strbuf->count + 1);
	*buf[new_length] = '\0';
	strbuf->count = new_length;
}

uint32_t
strbuf_capacity(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	return strbuf->capacity;
}

void
strbuf_get_chars(char **buf, uint32_t src_offset, uint32_t src_end, char *dst, uint32_t dst_offset)
{
	if (src_offset < src_end)
		memcpy(dst+dst_offset, buf+src_offset, src_end-src_offset);
}

uint32_t
strbuf_length(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	return strbuf->count;
}

void
strbuf_append_char(char **buf, char ch)
{
	StringBuffer *strbuf = get_string_buffer(buf);

	strbuf->count++;
	ensure_capacity(buf, &strbuf, strbuf->count + 1);
	(*buf)[strbuf->count-1] = ch;
	(*buf)[strbuf->count] = '\0';
}

void
strbuf_append(char **buf, const char *string)
{
	strbuf_append_substring(buf, string, 0, strlen(string));
}

void
strbuf_insert_char(char **buf, uint32_t offset, char ch)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	strbuf->count++;
	ensure_capacity(buf, &strbuf, strbuf->count + 1);
	memmove(*buf + offset + 1, *buf + offset, strbuf->count - offset);
	(*buf)[offset] = ch;
}

void
strbuf_insert(char **buf, uint32_t offset, const char *string)
{
	strbuf_insert_substring(buf, offset, string, 0, strlen(string));
}

int
strbuf_appendf(char **buf, const char *format, ...)
{
	va_list ap;
	int rc;
	char *tmp = NULL;

	va_start(ap, format);
	rc = vasprintf(&tmp, format, ap);
	if (rc > 0 && tmp != NULL)
		strbuf_append(buf, tmp);
	if (tmp != NULL)
		free(tmp);
	va_end(ap);

	return rc;
}

int
strbuf_insertf(char **buf, uint32_t offset, const char *format, ...)
{
	va_list ap;
	int rc;
	char *tmp = NULL;

	va_start(ap, format);
	rc = vasprintf(&tmp, format, ap);
	if (rc > 0 && tmp != NULL)
		strbuf_insert(buf, offset, tmp);
	if (tmp != NULL)
		free(tmp);
	va_end(ap);

	return rc;
}

void
strbuf_set(char **buf, const char *string)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	size_t len = strlen(string);

	strbuf->count = len;
	ensure_capacity(buf, &strbuf, strbuf->count + 1);
	memcpy(*buf, string, len + 1);
}

int
strbuf_setf(char **buf, const char *format, ...)
{
	va_list ap;
	int rc;
	char *tmp = NULL;

	va_start(ap, format);
	rc = vasprintf(&tmp, format, ap);
	if (rc > 0 && tmp != NULL)
		strbuf_set(buf, tmp);
	if (tmp != NULL)
		free(tmp);
	va_end(ap);

	return rc;
}

void
strbuf_delete(char **buf, uint32_t start, uint32_t end)
{
	StringBuffer *strbuf = get_string_buffer(buf);

/*	if (start < 0)
		start += strbuf->count;
	if (end < 0)
		end += strbuf->count;*/

	if (start < end) {
		memmove(*buf + start, *buf + end, strbuf->count - end + 1);
		strbuf->count -= end - start;
	}
}

void
strbuf_delete_char(char **buf, uint32_t offset)
{
	StringBuffer *strbuf = get_string_buffer(buf);

	if (offset < 0)
		offset += strbuf->count;

	memmove(*buf + offset, *buf + offset + 1, strbuf->count - offset + 1);
	strbuf->count--;
}

void
strbuf_replace(char **buf, uint32_t start, uint32_t end, const char *string)
{
	strbuf_replace_substring(buf, start, end, string, 0, strlen(string));
}

/**
 * Return a newly allocated regular string, while freeing the string
 * buffer.
 */
char *
strbuf_free_to_string(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	memmove(strbuf, *buf, strbuf->count + 1);
	return (char *) strbuf;
}

/**
 * Update the length of the buffer, so that the first null-byte
 * marks the end of the string buffer.
 *
 * This is useful when the string in the buffer has been truncated
 * or extended by some non-strbuf function.
 */
void
strbuf_update_length(char **buf)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	uint32_t c;

	for (c = 0; c < strbuf->capacity; c++) {
		if (buf[c] == '\0') {
			strbuf->count = c;
			return;
		}
	}
}

void
strbuf_replace_substring(char **buf, uint32_t start, uint32_t end,
		const char *string, uint32_t substart, uint32_t subend)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	size_t len = subend - substart;
	int32_t diff;

	/*if (start < 0)
		start += strbuf->count;
	if (end < 0)
		end += strbuf->count;*/

	diff = len - (end-start);
	if (diff < 0) {
		memmove(*buf+start+len, *buf+end, strbuf->count+1-end);
	} else if (diff > 0) {
		ensure_capacity(buf, &strbuf, strbuf->count+diff+1);
		memmove(*buf+start+len, *buf+end, strbuf->count+1-end);	
	}
	memcpy(*buf+start, string+substart, len);
	strbuf->count += diff;
}

char *
strbuf_new_from_substring(const char *string, uint32_t substart,
		uint32_t subend)
{
	char *buf;
	size_t len = subend - substart;

	buf = new_strbuf(len + 1 + DEFAULT_CAPACITY, len);
	memcpy(buf, string+substart, len);
	buf[len] = '\0';

	return buf;
}

void
strbuf_insert_substring(char **buf, uint32_t offset, const char *string,
		uint32_t substart, uint32_t subend)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	size_t len = subend - substart;

	ensure_capacity(buf, &strbuf, strbuf->count + len + 1);
	memmove(*buf + offset + len, *buf + offset, strbuf->count - offset + 1);
	memcpy(*buf + offset, string+substart, len);
	strbuf->count += len;
}

void
strbuf_append_substring(char **buf, const char *string,
		uint32_t substart, uint32_t subend)
{
	StringBuffer *strbuf = get_string_buffer(buf);
	size_t len = subend - substart;

	ensure_capacity(buf, &strbuf, strbuf->count + len + 1);
	memcpy(*buf + strbuf->count, string+substart, len);
	strbuf->count += len;
	(*buf)[strbuf->count] = '\0';
}

char *
strbuf_new_from_substring_to_end(const char *string, uint32_t substart)
{
	return strbuf_new_from_substring(string, substart, strlen(string));
}

void
strbuf_insert_substring_to_end(char **buf, uint32_t offset,
		const char *string,	uint32_t substart)
{
	return strbuf_insert_substring(buf, offset, string, substart,
			strlen(string));
}

void
strbuf_append_substring_to_end(char **buf, const char *string,
		uint32_t substart)
{
	return strbuf_append_substring(buf, string, substart, strlen(string));
}

inline void
strbuf_set_char_at(char **buf, uint32_t index, char ch)
{
	(*buf)[index] = ch;
}

inline void
strbuf_clear(char **buf)
{
	strbuf_set_length(buf, 0);
}
