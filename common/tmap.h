/* tmap.h - A red-black tree map implementation.
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

#ifndef COMMON_TMAP_H
#define COMMON_TMAP_H

#include <stdint.h>		/* POSIX/Gnulib */
#include "collections.h"
#include "iterator.h"

typedef struct _TMap TMap;

TMap *tmap_new(void);
TMap *tmap_new_specific(CompareFunc compare_keys);
uint32_t map_size(TMap *map);
bool tmap_contains_key(TMap *map, void *key);
bool tmap_contains_value(TMap *map, void *value);
void *tmap_first_key(TMap *map);
void *tmap_last_key(TMap *map);
void *tmap_get(TMap *map, void *key);
void *tmap_put(TMap *map, void *key, void *value);
void *tmap_remove(TMap *map, void *key);
void tmap_iterate_values(TMap *map, IteratorFunc iterator_func);
TMap *tmap_clone(TMap *map);

#endif
