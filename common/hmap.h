/* hmap.h - A hash table (map datatype) implementation.
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

#ifndef COMMON_HMAP_H
#define COMMON_HMAP_H

#include <stdint.h>		/* POSIX/Gnulib */
#include "collections.h"
#include "iterator.h"

typedef struct _HMap HMap;

typedef uint32_t (*HashFunc)(const void *);

HMap *hmap_new(void);
HMap *hmap_new_specific(uint32_t initial_capacity, float load_factor);
void hmap_set_hash_function(HMap *map, HashFunc hash_key);
void hmap_set_compare_function(HMap *map, CompareFunc compare_keys);
void hmap_free(HMap *map);

void hmap_clear(HMap *map);
uint32_t hmap_size(HMap *map);

bool hmap_is_empty(HMap *map);
bool hmap_contains_value(HMap *map, const void *value);
bool hmap_contains_key(HMap *map, const void *key);

void *hmap_get(HMap *map, const void *key);
void *hmap_put(HMap *map, void *key, void *value);
void *hmap_remove(HMap *map, const void *key);

uint32_t string_hash(const char *key);

void hmap_iterate_values(HMap *map, IteratorFunc iterator_func);
Iterator *hmap_value_iterator(HMap *map);

#endif
