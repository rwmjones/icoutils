/* tmap.c - A red-black tree map implementation.
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
#include <string.h>	/* C89 */
#include <stdlib.h>	/* C89 */
#include "xalloc.h"	/* Gnulib */
#include "tmap.h"

typedef struct _Node Node;

typedef enum {
	RED,
	BLACK,
} NodeColor;

struct _Node {
	void *key;
	void *value;
	NodeColor color;
	Node *left;
	Node *right;
	Node *parent;
};

struct _TMap {
	Node *root;
	uint32_t size;
	CompareFunc compare_keys;
};

static Node nil;

static Node *tmap_get_node(TMap *map, void *key);
static Node *tmap_first_node(TMap *map);
static Node *tmap_last_node(TMap *map);
static Node *successor(Node *node);
/*static Node *predecessor(Node *node);*/
static void tmap_insert_rebalance(TMap *map, Node *node);
static void tmap_rotate_left(TMap *map, Node *node);
static void tmap_rotate_right(TMap *map, Node *node);
static void tmap_delete_rebalance(TMap *map, Node *node);
static void tmap_remove_node(TMap *map, Node *node);
static void tmap_clear_nodes(TMap *map, Node *node);
static Node *tmap_clone_node(TMap *map, Node *node);

TMap *
tmap_new(void)
{
	return tmap_new_specific((CompareFunc) strcmp);
}

TMap *
tmap_new_specific(CompareFunc compare_keys)
{
	TMap *map = xmalloc(sizeof(TMap));

	map->size = 0;
	map->compare_keys = compare_keys;

	return map;
}

uint32_t
tmap_size(TMap *map)
{
	return map->size;
}

bool
tmap_contains_key(TMap *map, void *key)
{
	return tmap_get_node(map, key) != &nil;
}

bool
tmap_contains_value(TMap *map, void *value)
{
	Node *node = tmap_first_node(map);

	for (; node != &nil; node = successor(node)) {
		if (node->value == value)
			return true;
	}

	return false;
}

void *
tmap_first_key(TMap *map)
{
	return (map->root == &nil ? NULL : tmap_first_node(map)->key);
}

static Node *
tmap_first_node(TMap *map)
{
	Node *node = map->root;

	if (node == &nil)
		return &nil;
	while (node->left != &nil)
		node = node->left;
	return node;
}

void *
tmap_last_key(TMap *map)
{
	return (map->root == &nil ? NULL : tmap_last_node(map)->key);
}

static Node *
tmap_last_node(TMap *map)
{
	Node *node = map->root;

	if (node == &nil)
		return &nil;
	while (node->right != &nil)
		node = node->right;
	return node;
}

void *
tmap_get(TMap *map, void *key)
{
	return tmap_get_node(map, key)->value;
}

static Node *
tmap_get_node(TMap *map, void *key)
{
	Node *node = map->root;

	while (node != &nil) {
		int32_t compare = map->compare_keys(key, node->key);
		if (compare > 0)
			node = node->right;
		else if (compare < 0)
			node = node->left;
		else
			return node;
	}

	return node;
}

static Node *
successor(Node *node)
{
	Node *parent;

	if (node->right != &nil) {
		node = node->right;
		while (node->left != &nil)
			node = node->left;
		return node;
	}

	parent = node->parent;
	while (parent != &nil && node == parent->right) {
		node = parent;
		parent = parent->parent;
	}

	return parent;
}

/*
static Node *
predecessor(Node *node)
{
	Node *parent;

	if (node->left != &nil) {
		node = node->left;
		while (node->right != &nil)
			node = node->right;
		return node;
	}

	parent = node->parent;
	while (parent != &nil && node == parent->left) {
		node = parent;
		parent = parent->parent;
	}

	return parent;
}
*/

void *
tmap_put(TMap *map, void *key, void *value)
{
	Node *node = map->root;
	Node *parent = &nil;
	int compare = 0;

	while (node != &nil) {
		parent = node;
		compare = map->compare_keys(key, node->key);
		if (compare > 0) {
			node = node->right;
		} else if (compare < 0) {
			node = node->left;
		} else {
			void *old_value = node->value;
			node->value = value;
			return old_value;
		}
	}

	node = xmalloc(sizeof(Node));
	node->key = key;
	node->value = value;
	node->color = RED;
	node->parent = parent;
	node->left = &nil;
	node->right = &nil;

	map->size++;
	if (parent == &nil) {
		map->root = node;
		node->color = BLACK;
		return NULL;
	} else if (compare > 0) {
		parent->right = node;
	} else {
		parent->left = node;
	}

	tmap_insert_rebalance(map, node);
	return NULL;
}

static void
tmap_insert_rebalance(TMap *map, Node *node)
{
	while (node != map->root && node->parent->parent != &nil
			&& node->parent->color == RED) {

		if (node->parent == node->parent->parent->left) {
			Node *uncle = node->parent->parent->right;
			if (uncle != &nil && uncle->color == RED) {
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->right) {
					node = node->parent;
					tmap_rotate_left(map, node);
				}

				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				tmap_rotate_right(map, node->parent->parent);
			}
		} else {
			Node *uncle = node->parent->parent->left;
			if (uncle != &nil && uncle->color == RED) {
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				node = node->parent->parent;
			} else {
				if (node == node->parent->left) {
					node = node->parent;
					tmap_rotate_right(map, node);
				}

				node->parent->color = BLACK;
				node->parent->parent->color = RED;
				tmap_rotate_left(map, node->parent->parent);
			}
		}
	}

	map->root->color = BLACK;
}

static void
tmap_rotate_left(TMap *map, Node *node)
{
	Node *child = node->right;

	node->right = child->left;
	if (child->left != &nil)
		child->left->parent = node;

	child->parent = node->parent;
	if (node->parent != &nil) {
		if (node == node->parent->left)
			node->parent->left = child;
		else
			node->parent->right = child;
	} else {
		map->root = child;
	}
	
	child->left = node;
	if (node != &nil)
		node->parent = child;
}

static void
tmap_rotate_right(TMap *map, Node *node)
{
	Node *child = node->left;

	node->left = child->right;
	if (child->right != &nil)
		child->right->parent = node;

	child->parent = node->parent;
	if (node->parent != &nil) {
		if (node == node->parent->right)
			node->parent->right = child;
		else
			node->parent->left = child;
	} else {
		map->root = child;
	}
	
	child->right = node;
	if (node != &nil)
		node->parent = child;
}

void *
tmap_remove(TMap *map, void *key)
{
	Node *node = tmap_get_node(map, key);
	if (node != &nil) {
		void *value = node->value;
		tmap_remove_node(map, node);
		return value;
	}
	return NULL;
}

static void
tmap_remove_node(TMap *map, Node *node)
{
	Node *splice;
	Node *child;
	Node *parent;
	NodeColor splice_color;

	map->size--;

	if (node->left == &nil || node->right == &nil) {
		splice = node;
		if (node->left == &nil)
			child = node->right;
		else
			child = node->left;
	} else {
		splice = node->right;
		while (splice->left != &nil)
			splice = splice->left;
		child = splice->right;
	}

	parent = splice->parent;
	child->parent = parent;
	if (parent != &nil) {
		if (splice == parent->left)
			parent->left = child;
		else
			parent->right = child;
	} else {
		map->root = child;
	}

	splice_color = splice->color;
	if (splice != node) {
		if (node == map->root) {
			map->root = splice;
		} else {
			if (node->parent->left == node)
				node->parent->left = splice;
			else
				node->parent->right = splice;
		}
		splice->parent = node->parent;
		splice->left = node->left;
		splice->right = node->right;
		splice->left->parent = splice;
		splice->right->parent = splice;
		splice->color = node->color;
	}

	if (splice_color == BLACK)
		tmap_delete_rebalance(map, child);

	free(node);
}

static void
tmap_delete_rebalance(TMap *map, Node *node)
{
	while (node != map->root && node->color == BLACK) {

		if (node == node->parent->left) {
			Node *sibling = node->parent->right;
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				tmap_rotate_left(map, node->parent);
				sibling = node->parent->right;
			}

			if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
				sibling->color = RED;
				node = node->parent;
			} else {
				if (sibling->right->color == BLACK) {
					sibling->left->color = BLACK;
					sibling->color = RED;
					tmap_rotate_right(map, sibling);
					sibling = node->parent->right;
				}

				sibling->color = sibling->parent->color;
				sibling->parent->color = BLACK;
				sibling->right->color = BLACK;
				tmap_rotate_left(map, node->parent);
				node = map->root;
			}
		} else {
			Node *sibling = node->parent->left;
			if (sibling->color == RED) {
				sibling->color = BLACK;
				node->parent->color = RED;
				tmap_rotate_right(map, node->parent);
				sibling = node->parent->left;
			}

			if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
				sibling->color = RED;
				node = node->parent;
			} else {
				if (sibling->left->color == BLACK) {
					sibling->right->color = BLACK;
					sibling->color = RED;
					tmap_rotate_left(map, sibling);
					sibling = node->parent->left;
				}

				sibling->color = sibling->parent->color;
				sibling->parent->color = BLACK;
				sibling->left->color = BLACK;
				tmap_rotate_right(map, node->parent);
				node = map->root;
			}
		}
	}

	node->color = BLACK;
}

void
tmap_iterate_values(TMap *map, IteratorFunc iterator_func)
{
	Node *node = tmap_first_node(map);
	for (; node != &nil; node = successor(node))
		iterator_func(node->value);
}

void
tmap_clear(TMap *map)
{
	if (map->root != &nil) {
		tmap_clear_nodes(map, map->root);
		map->root = &nil;
	}
	map->size = 0;
}

static void
tmap_clear_nodes(TMap *map, Node *node)
{
	if (node->left != &nil)
		tmap_clear_nodes(map, node->left);
	if (node->right != &nil)
		tmap_clear_nodes(map, node->right);
	free(node);
}

TMap *
tmap_clone(TMap *map)
{
	TMap *copy = xmalloc(sizeof(TMap));

	copy->size = map->size;
	if (map->size == 0) {
		copy->root = &nil;
	} else {
		copy->root = tmap_clone_node(map, map->root);
		copy->root->parent = &nil;
	}
	return copy;
}

static Node *
tmap_clone_node(TMap *map, Node *node)
{
	Node *copy_node = xmalloc(sizeof(Node));

	copy_node->key = node->key;
	copy_node->value = node->value;
	copy_node->color = node->color;
	if (node->left != &nil) {
		copy_node->left = tmap_clone_node(map, node->left);
		copy_node->left->parent = copy_node;
	}
	if (node->right != &nil) {
		copy_node->right = tmap_clone_node(map, node->right);
		copy_node->right->parent = copy_node;
	}

	return copy_node;
}
