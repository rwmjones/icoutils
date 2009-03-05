/* tab-utils.c - Utility functions dealing with strings and tabs.
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
#include <string.h>	/* C89 */
#include "tab-utils.h"

/**
 * Output tab and space characters, so that when initially
 * at position `from', we will have moved to position `to'.
 * We try to maximize the number of tabs output, and
 * minimize the number of spaces.
 *
 * @param tabsize
 *   Length of a tab. A tabsize of 0 disables use of tab characters.
 */
void
tab_to(FILE *out, int from, int to, int tabsize)
{
	if (tabsize != 0) {
		while ((from/tabsize+1)*tabsize <= to) {
			putc('\t', out);
			from = (from/tabsize+1)*tabsize;
		}
	}
	for (; from < to; from++)
		putc(' ', out);
}

/**
 * Return the length of a string, considering tab characters as well.
 * The terminating null-byte is not counted.
 */
size_t
tab_len(const char *string, int tabsize)
{
	size_t len = 0;
	if (tabsize == 0)
		return strlen(string);
	for (; string[0] != '\0'; string++) {
		if (string[0] == '\t')
			len = (len/tabsize+1)*tabsize;
		else
			len++;
	}
	return len;
}

/**
 * Convert an index that takes into account tab-characters to an
 * index of a character in the specified string.
 *
 * @param pos
 *   Must be positive.
 */ 
int
tab_index(const char *string, int pos, int tabsize)
{
	int c = 0;
	int p;

	if (tabsize == 0)
		return pos;
	for (p = 0; c < pos && string[p] != '\0'; p++) {
		if (string[p] == '\t')
			c = (c/tabsize+1)*tabsize;
		else
			c++;
	}
	return (c > pos ? p-1 : p);
}

/**
 * Return the character at the specified position, considering tab
 * characters as well.
 */ 
inline char
tab_char_at(const char *string, int pos, int tabsize)
{
	return string[tab_index(string, pos, tabsize)];
}
