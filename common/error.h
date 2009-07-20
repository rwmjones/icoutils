/* error.c - Error-management and messaging routines.
 *
 * Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2008
 * Oskar Liljeblad
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

#include <stdarg.h>	/* Gnulib/C89 */
#include <stddef.h>	/* C89 */
#include <errno.h>	/* C89 */

extern void (*program_termination_hook)(void);

void internal_error(const char *msg, ...) __attribute__ ((noreturn));
void die(const char *msg, ...) __attribute__ ((noreturn));
void die_errno(const char *msg, ...) __attribute__ ((noreturn));
void warn(const char *msg, ...);
void warn_errno(const char *msg, ...);
void set_message_header(const char *msg, ...);
void restore_message_header(void);

void set_error(const char *msg, ...);
const char *get_error(void);
char *remove_error(void);
void die_error(void) __attribute__ ((noreturn));

void free_error(void);

#define errstr strerror(errno)

#endif
