/* intparse.c - Functions to parse integers from strings.
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
#include <stdint.h>		/* POSIX/Gnulib */
#include "intparse.h"

bool
parse_int8(const char *instr, int8_t *outint)
{
	int8_t value = 0;

	if (*instr == '-') {
		for (instr++; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value < INT8_MIN/10 || (value == INT8_MIN/10 && c > -(INT8_MIN%10)))
				return false;
			value = value*10 - c;
		}
	} else {
		for (; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value > INT8_MAX/10 || (value == INT8_MAX/10 && c > INT8_MAX%10))
				return false;
			value = value*10 + c;
		}
	}
	*outint = value;

	return true;
}

bool
parse_int16(const char *instr, int16_t *outint)
{
	int16_t value = 0;

	if (*instr == '-') {
		for (instr++; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value < INT16_MIN/10 || (value == INT16_MIN/10 && c > -(INT16_MIN%10)))
				return false;
			value = value*10 - c;
		}
	} else {
		for (; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value > INT16_MAX/10 || (value == INT16_MAX/10 && c > INT16_MAX%10))
				return false;
			value = value*10 + c;
		}
	}
	*outint = value;

	return true;
}

bool
parse_int32(const char *instr, int32_t *outint)
{
	int32_t value = 0;

	if (*instr == '-') {
		if (instr[1] == '\0')		/* FIXME: this should be done on all!!! */
			return false;
		for (instr++; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value < INT32_MIN/10L || (value == INT32_MIN/10L && c > -(INT32_MIN%10L)))
				return false;
			value = value*10L - c;
		}
	} else {
		if (*instr == '\0')			/* FIXME: this should be done on all!!! */
			return false;
		for (; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value > INT32_MAX/10L || (value == INT32_MAX/10L && c > INT32_MAX%10L))
				return false;
			value = value*10L + c;
		}
	}
	*outint = value;

	return true;
}

bool
parse_int64(const char *instr, int64_t *outint)
{
	int64_t value = 0;

	if (*instr == '-') {
		for (instr++; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value < INT64_MIN/10LL || (value == INT64_MIN/10LL && c > -(INT64_MIN%10LL)))
				return false;
			value = value*10LL - c;
		}
	} else {
		for (; *instr != '\0'; instr++) {
			int8_t c = *instr - '0';
			if (c < 0 || c > 9)
				return false;
			if (value > INT64_MAX/10LL || (value == INT64_MAX/10LL && c > INT64_MAX%10LL))
				return false;
			value = value*10LL + c;
		}
	}
	*outint = value;

	return true;
}

bool
parse_uint8(const char *instr, uint8_t *outint)
{
	uint8_t value = 0;

	for (; *instr != '\0'; instr++) {
		int8_t c = *instr - '0';
		if (c < 0 || c > 9)
			return false;
		if (value > UINT8_MAX/10 || (value == UINT8_MAX/10 && c > UINT8_MAX%10))
			return false;
		value = value*10 + c;
	}
	*outint = value;

	return true;
}

bool
parse_uint16(const char *instr, uint16_t *outint)
{
	uint16_t value = 0;

	for (; *instr != '\0'; instr++) {
		int8_t c = *instr - '0';
		if (c < 0 || c > 9)
			return false;
		if (value > UINT16_MAX/10 || (value == UINT16_MAX/10 && c > UINT16_MAX%10))
			return false;
		value = value*10 + c;
	}
	*outint = value;

	return true;
}

bool
parse_uint32(const char *instr, uint32_t *outint)
{
	uint32_t value = 0;

	for (; *instr != '\0'; instr++) {
		int8_t c = *instr - '0';
		if (c < 0 || c > 9)
			return false;
		if (value > UINT32_MAX/10L || (value == UINT32_MAX/10L && c > UINT32_MAX%10))
			return false;
		value = value*10L + c;
	}
	*outint = value;

	return true;
}

bool
parse_uint64(const char *instr, uint64_t *outint)
{
	uint64_t value = 0;

	for (; *instr != '\0'; instr++) {
		int8_t c = *instr - '0';
		if (c < 0 || c > 9)
			return false;
		if (value > UINT64_MAX/10LL || (value == UINT64_MAX/10LL && c > UINT64_MAX%10LL))
			return false;
		value = value*10LL + c;
	}
	*outint = value;

	return true;
}
