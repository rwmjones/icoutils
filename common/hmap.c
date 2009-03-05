/* hmap.c - A hash table (map datatype) implementation.
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
#include <stdlib.h>		/* C89 */
#include <string.h>		/* C89 */
#include <stdio.h>		/* C89 */
#include "xalloc.h"		/* Gnulib */
#include "hmap.h"
#include "error.h"
#include "memory.h"

#define DEFAULT_CAPACITY		11
#define DEFAULT_LOAD_FACTOR		0.75F

typedef struct _HMapIterator HMapIterator;

typedef struct _Entry Entry;

struct _Entry {
	void *key;
	void *value;
	Entry *next;
};

struct _HMap {
	Entry **buckets;
	uint32_t buckets_length;
	uint32_t threshold;
	float load_factor;
	uint32_t size;
	
	HashFunc hash_key;
	CompareFunc compare_keys;
};

struct _HMapIterator {
	Iterator iterator;
	HMap *map;
	uint32_t index;
	Entry *entry;
	Entry *previous_entry;
};

static inline uint32_t hmap_hash(HMap *map, const void *key);
static inline void hmap_rehash(HMap *map);
static bool hmap_iterator_has_next(Iterator *it);
static void *hmap_iterator_next(Iterator *it);
static void hmap_iterator_remove(Iterator *it);
static void hmap_iterator_restart(Iterator *iterator);

static IteratorClass hmap_iterator_class = {
	hmap_iterator_has_next,
	hmap_iterator_next,
	hmap_iterator_remove,
	NULL,
	hmap_iterator_restart,
	NULL,
	NULL,
};

uint32_t
string_hash(const char *key)
{
	uint32_t hash = 0;

	for (; *key != '\0'; key++)
		hash = (hash << 5) - hash + *key;

	return hash;
}

HMap *
hmap_new(void)
{
	return hmap_new_specific(DEFAULT_CAPACITY, DEFAULT_LOAD_FACTOR);
}

HMap *
hmap_new_specific(uint32_t initial_capacity, float load_factor)
{
	HMap *map = xmalloc(sizeof(HMap));

	map->buckets = xmalloc(initial_capacity * sizeof(Entry *));
	memclear(map->buckets, initial_capacity * sizeof(Entry *));

	map->buckets_length = initial_capacity;
	map->threshold = (uint32_t) (initial_capacity * load_factor);
	map->load_factor = load_factor;
	map->size = 0;

	map->hash_key = (HashFunc) string_hash;
	map->compare_keys = (CompareFunc) strcmp;

	return map;
}

void
hmap_set_hash_function(HMap *map, HashFunc hash_key)
{
	map->hash_key = hash_key;
}

void
hmap_set_compare_function(HMap *map, CompareFunc compare_keys)
{
	map->compare_keys = compare_keys;
}

uint32_t
hmap_size(HMap *map)
{
	return map->size;
}

bool
hmap_is_empty(HMap *map)
{
	return map->size == 0;
}

bool
hmap_contains_value(HMap *map, const void *value)
{
	uint32_t i;

	for (i = 0; i < map->buckets_length; i++) {
		Entry *entry;
		for (entry = map->buckets[i]; entry != NULL; entry = entry->next) {
			if (value == entry->value)
				return true;
		}
	}

	return false;
}

bool
hmap_contains_key(HMap *map, const void *key)
{
	if (key == NULL) {
		Entry *entry = map->buckets[0];
		for (; entry != NULL; entry = entry->next) {
			if (entry->key == NULL)
				return true;
		}
	} else {
		Entry *entry = map->buckets[hmap_hash(map, key)];
		for (; entry != NULL; entry = entry->next) {
//			printf("compare %s vs %s -> %d\n",key,entry->key,map->compare_keys(key, entry->key));
			if (map->compare_keys(key, entry->key) == 0)
				return true;
		}
	}

	return false;
}

static inline uint32_t
hmap_hash(HMap *map, const void *key)
{
//	printf("hash %s into -> %d pos %d (%d)\n", key, map->hash_key(key), map->hash_key(key) % map->buckets_length, map->buckets_length);
	return (key == NULL ? 0 : (map->hash_key(key) % map->buckets_length));
}


/*static void
hmap_dump(HMap *map)
{
	uint32_t i;
	printf("hmap_dump(map=%p)\n", map);
	for (i = 0; i < map->buckets_length; i++) {
		Entry *e = map->buckets[i];
		while (e != NULL) {
			printf("index=%d entry=%p key=%p value=%p\n", i, e, e->key, e->value);
			e = e->next;
		}
	}
	printf("\n");
}*/

void *
hmap_get(HMap *map, const void *key)
{
	if (key == NULL) {
		Entry *entry = map->buckets[0];
		for (; entry != NULL; entry = entry->next) {
			if (entry->key == NULL)
				return entry->value;
		}
	} else {
		Entry *entry = map->buckets[hmap_hash(map, key)];
		for (; entry != NULL; entry = entry->next) {
			if (map->compare_keys(key, entry->key) == 0)
				return entry->value;
		}
	}

	return NULL;
}

void *
hmap_put(HMap *map, void *key, void *value)
{
	Entry *entry;
	uint32_t index;

	if (key == NULL) {
		for (entry = map->buckets[0]; entry != NULL; entry = entry->next) {
			if (entry->key == NULL) {
				void *old_value = entry->value;
				entry->value = value;
				return old_value;
			}
		}
		index = 0;
	} else {
		index = hmap_hash(map, key);
		for (entry = map->buckets[index]; entry != NULL; entry = entry->next) {
			if (map->compare_keys(key, entry->key) == 0) {
				void *old_value = entry->value;
				entry->value = value;
				return old_value;
			}
		}
	}

	map->size++;
	if (map->size > map->threshold) {
		hmap_rehash(map);
		index = hmap_hash(map, key);
	}

	entry = xmalloc(sizeof(Entry));
	entry->key = key;
	entry->value = value;
	entry->next = map->buckets[index];
	map->buckets[index] = entry;

	return NULL;
}

void *
hmap_remove(HMap *map, const void *key)
{
	Entry *entry;
	Entry *last = NULL;

	if (key == NULL) {
		for (entry = map->buckets[0]; entry != NULL; entry = entry->next) {
			if (entry->key == NULL) {
				void *value = entry->value;
				if (last == NULL)
					map->buckets[0] = entry->next;
				else
					last->next = entry->next;
				map->size--;
				free(entry);
				return value;
			}
			last = entry;
		}
	} else {
		uint32_t index = hmap_hash(map, key);
		for (entry = map->buckets[index]; entry != NULL; entry = entry->next) {
			if (map->compare_keys(key, entry->key) == 0) {
				void *value = entry->value;
				if (last == NULL)
					map->buckets[index] = entry->next;
				else
					last->next = entry->next;
				map->size--;
				free(entry);
				return value;
			}
			last = entry;
		}
	}

	return NULL;
}

void
hmap_clear(HMap *map)
{
	uint32_t i;

    for (i = 0; i < map->buckets_length; i++) {
		Entry *entry = map->buckets[i];
		while (entry != NULL) {
			Entry *next = entry->next;
			free(entry);
			entry = next;
		}
		map->buckets[i] = NULL;
	}

	map->size = 0;
}

void
hmap_free(HMap *map)
{
	hmap_clear(map);
	free(map->buckets);
	free(map);
}

HMap *
hmap_clone(HMap *map)
{
	HMap *copy = xmalloc(sizeof(HMap));
	uint32_t i;

	memcpy(copy, map, sizeof(HMap));
	copy->buckets = xmalloc(map->buckets_length * sizeof(Entry *));

    for (i = 0; i < map->buckets_length; i++) {
		Entry *entry;
		Entry *last = NULL;

		for (entry = map->buckets[i]; entry != NULL; entry = entry->next) {
			Entry *new_entry = xmalloc(sizeof(Entry));
			new_entry->key = entry->key;
			new_entry->value = entry->value;
			new_entry->next = NULL;

			if (last == NULL)
				copy->buckets[i] = last = new_entry;
			else
				last->next = new_entry;
				last = new_entry;
		}
	}

	return copy;
}

static inline void
hmap_rehash(HMap *map)
{	
	Entry **old_buckets = map->buckets;
	uint32_t old_capacity = map->buckets_length;
	uint32_t i;

	map->buckets_length = (map->buckets_length * 2) + 1;
	map->threshold = (uint32_t) (map->buckets_length * map->load_factor);
	map->buckets = xmalloc(map->buckets_length * sizeof(Entry *));
	memclear(map->buckets, map->buckets_length * sizeof(Entry *));

	for (i = 0; i < old_capacity; i++) {
		Entry *entry = old_buckets[i];
		while (entry != NULL) {
			uint32_t index = hmap_hash(map, entry->key);
			Entry *dest = map->buckets[index];
			Entry *next;

			if (dest != NULL) {
				while (dest->next != NULL)
					dest = dest->next;
				dest->next = entry;
			} else {
				map->buckets[index] = entry;
			}

			next = entry->next;
			entry->next = NULL;
			entry = next;
		}
	}

	free(old_buckets);
}

void
hmap_iterate_values(HMap *map, IteratorFunc iterator_func)
{
	uint32_t i;

    for (i = 0; i < map->buckets_length; i++) {
		Entry *entry;
		for (entry = map->buckets[i]; entry != NULL; entry = entry->next)
			iterator_func(entry->value);
	}
}

Iterator *
hmap_value_iterator(HMap *map)
{
	HMapIterator *it = xmalloc(sizeof(HMapIterator));
	it->iterator.class = &hmap_iterator_class;
	it->map = map;
	hmap_iterator_restart(&it->iterator);
	return &it->iterator;
}

static bool
hmap_iterator_has_next(Iterator *it)
{
	return ((HMapIterator *) it)->entry != NULL;
}

static void *
hmap_iterator_next(Iterator *iterator)
{
	HMapIterator *it = (HMapIterator *) iterator;
	HMap *map = it->map;
	void *data;

	if (it->entry == NULL)
		return NULL;

	data = it->entry->value;
	it->previous_entry = it->entry;

	it->entry = it->entry->next;
	if (it->entry == NULL) {
		uint32_t i = it->index + 1;
		for (; i < map->buckets_length && map->buckets[i] == NULL; i++);
		it->index = i;
		it->entry = (i < map->buckets_length ? map->buckets[i] : NULL);
	}

	return data;
}

static void
hmap_iterator_remove(Iterator *iterator)
{
	HMapIterator *it = (HMapIterator *) iterator;
	if (it->previous_entry != NULL) {
		hmap_remove(it->map, it->previous_entry->key);
		it->previous_entry = NULL;
	}
}

static void
hmap_iterator_restart(Iterator *iterator)
{
	HMapIterator *it = (HMapIterator *) iterator;
	uint32_t i;

	for (i = 0; i < it->map->buckets_length && it->map->buckets[i] == NULL; i++);
	it->index = i;
	it->entry = (i < it->map->buckets_length ? it->map->buckets[i] : NULL);
	it->previous_entry = NULL;
}
