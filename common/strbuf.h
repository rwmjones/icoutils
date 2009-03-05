/* strbuf.h - The string buffer data-structure.
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

#ifndef COMMON_STRBUF_H
#define COMMON_STRBUF_H

#include <sys/types.h>		/* POSIX */
#include <stdint.h>		/* POSIX/Gnulib */
#include <stdarg.h>		/* C89 */

/**
 * A buffer holding any characters of any length, even null-bytes.
 * The buffer is however always null-terminated.
 *
 * A user may modify any characters in the buffer.
 */

void strbuf_append_char(char **buf, char ch);
void strbuf_append(char **buf, const char *string);
int strbuf_appendf(char **buf, const char *format, ...);

void strbuf_insert_char(char **buf, uint32_t offset, char ch);
void strbuf_insert(char **buf, uint32_t offset, const char *string);
int strbuf_insertf(char **buf, uint32_t offset, const char *format, ...);

#define strbuf_prepend_char(buf,ch) strbuf_insert_char(buf,0,ch)
#define strbuf_prepend(buf,string) strbuf_append(buf,string)
#define strbuf_appendf(buf,format,...) strbuf_appendf(buf,format,__VA_ARGS__)

void strbuf_delete(char **buf, uint32_t start, uint32_t end);
void strbuf_delete_char(char **buf, uint32_t index);
void strbuf_replace(char **buf, uint32_t start, uint32_t end, const char *string);

uint32_t strbuf_length(char **buf);	/* strlen won't check beyond nullbytes */
void strbuf_set_length(char **buf, uint32_t new_length);
void strbuf_update_length(char **buf);
uint32_t strbuf_capacity(char **buf);
void strbuf_ensure_capacity(char **buf, uint32_t minimum_capacity);

char *strbuf_new(void);
char *strbuf_new_with_capacity(uint32_t capacity);
char *strbuf_new_from_string(const char *str);

void strbuf_free(char **buf);
char *strbuf_free_to_string(char **buf);

char *strbuf_substring_to_end(char **buf, uint32_t beginIndex);
char *strbuf_substring(char **buf, uint32_t beginIndex, uint32_t endIndex);
void strbuf_get_chars(char **buf, uint32_t src_offset, uint32_t src_end, char *dst, uint32_t dst_offset);
char *strbuf_to_string(char **buf);

void strbuf_set(char **buf, const char *string);
int strbuf_setf(char **buf, const char *format, ...);

void strbuf_replace_substring(char **buf, uint32_t start, uint32_t end,
		const char *string, uint32_t substart, uint32_t subend);
char *strbuf_new_from_substring(const char *string, uint32_t substart,
		uint32_t subend);
void strbuf_insert_substring(char **buf, uint32_t offset, const char *string,
		uint32_t substart, uint32_t subend);
void strbuf_append_substring(char **buf, const char *string,
		uint32_t substart, uint32_t subend);
#define strbuf_prepend_substring(buf,string,substart,subend) \
		strbuf_insert_substring(buf,0,string,substart,subend)

char *strbuf_new_from_substring_to_end(const char *string, uint32_t substart);
void strbuf_insert_substring_to_end(char **buf, uint32_t offset,
		const char *string,	uint32_t substart);
void strbuf_append_substring_to_end(char **buf, const char *string,
		uint32_t substart);
#define strbuf_prepend_substring_to_end(buf,string,substart) \
		strbuf_insert_substring_to_end(buf,0,string,substart)

extern inline char
strbuf_char_at(char **buf, uint32_t index)
{
	return (*buf)[index];
}

extern inline void
strbuf_set_char_at(char **buf, uint32_t index, char ch)
{
	(*buf)[index] = ch;
}

extern inline void
strbuf_clear(char **buf)
{
	strbuf_set_length(buf, 0);
}

#endif
