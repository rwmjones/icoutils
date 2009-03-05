/* palette.c - Palette hash management for icon/cursor creation
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

#include <config.h>
#include <stdint.h>		/* POSIX/Gnulib */
#include <stdlib.h>		/* C89 */
#include "icotool.h"
#include "common/hmap.h"
#include "common/memory.h"
#include "common/common.h"

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t index;
} PaletteColor;

struct _Palette {
	HMap *map;
	Iterator *it;
};

static uint32_t
color_hash(PaletteColor *color)
{
	return (color->red << 16) | (color->green << 8) | color->blue;
}

static int32_t
color_compare(PaletteColor *c1, PaletteColor *c2)
{
	if (c1->red != c2->red)
		return c1->red - c2->red;
	if (c1->green != c2->green)
		return c1->green - c2->green;
	if (c1->blue != c2->blue)
		return c1->blue - c2->blue;
	return 0;
}

Palette *
palette_new(void)
{
	Palette *palette = xmalloc(sizeof(Palette));
	palette->map = hmap_new();
	palette->it = NULL;
	hmap_set_hash_function(palette->map, (HashFunc) color_hash);
	hmap_set_compare_function(palette->map, (CompareFunc) color_compare);
	return palette;
}

void
palette_free(Palette *palette)
{
	if (palette->it != NULL)
		iterator_free(palette->it);
	hmap_iterate_values(palette->map, free);
	hmap_free(palette->map);
	free(palette);
}

void
palette_add(Palette *palette, uint8_t r, uint8_t g, uint8_t b)
{
	PaletteColor color = { r, g, b, 0 };

	if (!hmap_contains_key(palette->map, &color)) {
		PaletteColor *new_color = xmalloc(sizeof(PaletteColor));
		new_color->red = r;
		new_color->green = g;
		new_color->blue = b;
		new_color->index = 0;
		hmap_put(palette->map, new_color, new_color);
	}
}

bool
palette_next(Palette *palette, uint8_t *r, uint8_t *g, uint8_t *b)
{
	if (palette->it == NULL)
		palette->it = hmap_value_iterator(palette->map);
	if (iterator_has_next(palette->it)) {
		PaletteColor *color = iterator_next(palette->it);
		*r = color->red;
		*g = color->green;
		*b = color->blue;
		return true;
	}
	iterator_free(palette->it);
	palette->it = NULL;
	return false;
}

void
palette_assign_indices(Palette *palette)
{
	Iterator *it = hmap_value_iterator(palette->map);
	uint32_t c;

	for (c = 0; iterator_has_next(it); c++) {
		PaletteColor *color = iterator_next(it);
		color->index = c;
	}
}

uint32_t
palette_lookup(Palette *palette, uint8_t r, uint8_t g, uint8_t b)
{
	PaletteColor color = { r, g, b, 0 };
	PaletteColor *real_color = hmap_get(palette->map, &color);
	return (real_color != NULL ? real_color->index : -1);
}

uint32_t
palette_count(Palette *palette)
{
	return hmap_size(palette->map);
}
