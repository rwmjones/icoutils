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

#ifndef COMMON_ALIST_H
#define COMMON_ALIST_H

#include <sys/types.h>		/* POSIX */
#include <stdint.h>		/* POSIX/Gnulib */
#include <stdbool.h>		/* POSIX/Gnulib */
#include "iterator.h"

typedef struct _AList AList;

AList *alist_new(void);
AList *alist_new_with_capacity(uint32_t capacity);
void alist_free(AList *list);
void alist_ensure_capacity(AList *list, uint32_t min_capacity);
void alist_add(AList *list, void *data);
void *alist_get(AList *list, uint32_t index);
uint32_t alist_size(AList *list);
void *alist_remove_at(AList *list, uint32_t index);
void alist_add_at(AList *list, uint32_t index, void *data);
Iterator *alist_iterator(AList *list);
bool alist_contains(AList *list, void *data);
uint32_t alist_index_of(AList *list, void *data);
uint32_t alist_last_index_of(AList *list, void *data);
void alist_clear(AList *list);
void *alist_set(AList *list, uint32_t index, void *data);
void **alist_to_array(AList *list);
void **alist_to_null_terminated_array(AList *list);
void alist_trim_to_size(AList *list);
bool alist_is_empty(AList *list);
void *alist_remove_last(AList *list);
void *alist_get_last(AList *list);
void alist_iterate(AList *list, IteratorFunc iterator_func);

#endif
