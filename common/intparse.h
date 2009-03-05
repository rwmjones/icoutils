/* intparse.h - Functions to parse integers from strings.
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

#ifndef COMMON_INTPARSE_H
#define COMMON_INTPARSE_H

#include <stdbool.h>	/* POSIX/gnulib */
#include <stdint.h>	/* POSIX/gnulib */

bool parse_int8(const char *instr, int8_t *outint);
bool parse_int16(const char *instr, int16_t *outint);
bool parse_int32(const char *instr, int32_t *outint);
bool parse_int64(const char *instr, int64_t *outint);

bool parse_uint8(const char *instr, uint8_t *outint);
bool parse_uint16(const char *instr, uint16_t *outint);
bool parse_uint32(const char *instr, uint32_t *outint);
bool parse_uint64(const char *instr, uint64_t *outint);

#endif
