/* alist.h - An array-based list with a container object (unlike GList)
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
#include "xalloc.h"		/* Gnulib */
#include "iterator.h"
#include "alist.h"
#include "memory.h"

#define DEFAULT_CAPACITY 16

typedef struct _AListIterator AListIterator;

struct _AList {
	uint32_t size;
	uint32_t capacity;
	void **data;
};

struct _AListIterator {
	Iterator iterator;
	AList *list;
	uint32_t index;
};

static void alist_iterator_remove(Iterator *iterator);
static void *alist_iterator_next(Iterator *iterator);
static bool alist_iterator_has_next(Iterator *iterator);
static void alist_iterator_restart(Iterator *iterator);

static IteratorClass alist_iterator_class = {
	alist_iterator_has_next,
	alist_iterator_next,
	alist_iterator_remove,
	NULL,
	alist_iterator_restart,
	NULL,
	NULL,
};

AList *
alist_new_with_capacity(uint32_t capacity)
{
	AList *list = xmalloc(sizeof(AList));

	list->size = 0;
	list->capacity = capacity;
	list->data = xmalloc(capacity * sizeof(void *));

	return list;
}

AList *
alist_new(void)
{
	return alist_new_with_capacity(DEFAULT_CAPACITY);
}

void
alist_ensure_capacity(AList *list, uint32_t min_capacity)
{
	if (min_capacity > list->capacity) {
		uint32_t new_capacity = MAX(list->capacity * 2, min_capacity);
		list->data = xrealloc(list->data, new_capacity * sizeof(void *));
		list->capacity = new_capacity;
	}
}

void
alist_add(AList *list, void *data)
{
	if (list->size == list->capacity)
		alist_ensure_capacity(list, list->size + 1);
	list->data[list->size++] = data;
}

void *
alist_get(AList *list, uint32_t index)
{
	return (index < list->size ? list->data[index] : NULL);
}

uint32_t
alist_size(AList *list)
{
	return list->size;
}

void *
alist_remove_at(AList *list, uint32_t index)
{
	void *data;

	if (index >= list->size)
		return NULL;

	data = list->data[index];
	list->size--;
	if (index != list->size) {
		memmove(list->data+index, list->data+index+1,
				sizeof(void *) * (list->size - index));
	}
	return data;
}

void
alist_add_at(AList *list, uint32_t index, void *data)
{
	if (index > list->size)
		return;

	if (list->size == list->capacity)
		alist_ensure_capacity(list, list->size + 1);
	if (index != list->size) {
		memmove(list->data+index+1, list->data+index,
				sizeof(void *) * (list->size - index));
	}
	list->data[index] = data;
	list->size++;
}

void
alist_free(AList *list)
{
	free(list->data);
	free(list);
}

AList *
alist_clone(AList *list)
{
	AList *copy = xmalloc(sizeof(AList));
	
	memcpy(copy, list, sizeof(AList));
	copy->data = xmalloc(copy->capacity * sizeof(void *));
	memcpy(copy->data, list->data, copy->size * sizeof(void *));

	return copy;
}

Iterator *
alist_iterator(AList *list)
{
	AListIterator *it = xmalloc(sizeof(AListIterator));	
	it->iterator.class = &alist_iterator_class;
	it->list = list;
	it->index = 0;
	return &it->iterator;
}

static bool
alist_iterator_has_next(Iterator *iterator)
{
	AListIterator *it = (AListIterator *) iterator;
	return it->index < it->list->size;
}

static void *
alist_iterator_next(Iterator *iterator)
{
	AListIterator *it = (AListIterator *) iterator;
	return it->list->data[it->index++];
}

static void
alist_iterator_remove(Iterator *iterator)
{
	AListIterator *it = (AListIterator *) iterator;
	if (it->index > 0)
		alist_remove_at(it->list, it->index-1);
}

static void
alist_iterator_restart(Iterator *iterator)
{
	AListIterator *it = (AListIterator *) iterator;
	it->index = 0;
}

bool
alist_contains(AList *list, void *data)
{
	return alist_index_of(list, data) != -1;
}

uint32_t
alist_index_of(AList *list, void *data)
{
	uint32_t c;

	if (data == NULL) {
		for (c = 0; c < list->size; c++) {
			if (list->data[c] == NULL)
				return c;
		}
	} else {
		for (c = 0; c < list->size; c++) {
			if (list->data[c] == data)
				return c;
		}
	}

	return -1;
}

uint32_t
alist_last_index_of(AList *list, void *data)
{
	uint32_t c;

	if (data == NULL) {
		for (c = list->size-1; c >= 0; c--) {
			if (list->data[c] == NULL)
				return c;
		}
	} else {
		for (c = list->size-1; c >= 0; c--) {
			if (list->data[c] == data)
				return c;
		}
	}

	return -1;
}

void
alist_clear(AList *list)
{
	memclear(list->data, list->size * sizeof(void *));
	list->size = 0;
}

void *
alist_set(AList *list, uint32_t index, void *data)
{
	void *old_data;

	if (index >= list->size)
		return NULL;

	old_data = list->data[index];
	list->data[index] = data;
	return old_data;
}

void **
alist_to_array(AList *list)
{
	void **array = xmalloc(list->size * sizeof(void *));
	/* FIXME: what happens if list->size == 0? also check for LList! */
	memcpy(array, list->data, list->size * sizeof(void *));
	return array;
}

void **
alist_to_null_terminated_array(AList *list)
{
	void **array = xmalloc((list->size+1) * sizeof(void *));
	memcpy(array, list->data, list->size * sizeof(void *));
	array[list->size] = NULL;
	return array;
}

void
alist_trim_to_size(AList *list)
{
	list->data = xrealloc(list->data, list->size * sizeof(void *));
	list->capacity = list->size;
}

bool
alist_is_empty(AList *list)
{
    return list->size == 0;
}


void *
alist_remove_last(AList *list)
{
    return list->data[--list->size];
}

void *
alist_get_last(AList *list)
{
    return list->data[list->size-1];
}

void
alist_iterate(AList *list, IteratorFunc iterator_func)
{
    int c;
    for (c = 0; c < list->size; c++)
    	iterator_func(list->data[c]);
}
