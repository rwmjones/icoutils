/* time64.c - Complete time: microseconds and seconds condensed into an int64_t
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
#include <stdint.h>	/* POSIX/Gnulib */
#include "time64.h"	/* common */

inline int64_t
time64_seconds(int64_t time)
{
	return time / MICROS_PER_SEC;
}

inline int32_t
time64_microseconds(int64_t time)
{
	return time % MICROS_PER_SEC;
}

inline int64_t
time64_from_timeval(struct timeval *tv)
{
	return tv->tv_sec * MICROS_PER_SEC + tv->tv_usec;
}

inline void
time64_to_timeval(struct timeval *tv, int64_t time)
{
	tv->tv_sec = time / MICROS_PER_SEC;
	tv->tv_usec = time % MICROS_PER_SEC;
}

inline int64_t
time64_now(void)
{
	struct timeval tv;
	if (gettimeofday(&tv, (struct timezone *) 0) < 0)
		return 0LL;
	return time64_from_timeval(&tv);
}
