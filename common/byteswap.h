/* byteswap.h - Byte swapping
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


#ifndef COMMON_BYTESWAP_H

#include <config.h>

#if HAVE_BYTESWAP_H
#include <byteswap.h>
#else
#define bswap_16(x) ((((x) & 0x00FF) << 8) | \
                     (((x) & 0xFF00) >> 8))
#define bswap_32(x) ((((x) & 0x000000FF) << 24) | \
                     (((x) & 0x0000FF00) << 8) | \
                     (((x) & 0x00FF0000) << 8) | \
                     (((x) & 0xFF000000) >> 24))
#define bswap_64(x) ((((x) & 0x00000000000000FFULL) << 56) | \
                     (((x) & 0x000000000000FF00ULL) << 40) | \
                     (((x) & 0x0000000000FF0000ULL) << 24) | \
                     (((x) & 0x00000000FF000000ULL) << 8) | \
		     (((x) & 0x000000FF00000000ULL) >> 8) | \
                     (((x) & 0x0000FF0000000000ULL) >> 24) | \
                     (((x) & 0x00FF000000000000ULL) >> 40) | \
                     (((x) & 0xFF00000000000000ULL) >> 56))
#endif

#endif
