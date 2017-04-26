/*
 * lib/rbtree.c
 *
 * Copyright (c) 2017 Baruch Marcot
 *
 * This red-black tree library implements CLRS's algorithm as described
 * in their book "Introduction to Algorithms - 3rd Edition". User must
 * provide an insertion function that will insert the new node as a leaf,
 * and call the fixup function to rebalance the tree.
 */

#include "rbtree.h"

static void rotate_left(struct rb_tree *t, struct rb_node *x)
{
	struct rb_node *y;

	y = x->right;
	x->right = y->left;
	if (y->left != &t->nil)
		rb_set_parent(y->left, x);
	rb_set_parent(y, RB_PARENT(x));
	if (RB_PARENT(x) == &t->nil) {
		t->root = y;
	} else if (x == RB_PARENT(x)->left) {
		RB_PARENT(x)->left = y;
	} else {
		RB_PARENT(x)->right = y;
	}
	y->left = x;
	rb_set_parent(x, y);
}

static void rotate_right(struct rb_tree *t, struct rb_node *x)
{
	struct rb_node *y;

	y = x->left;
	x->left = y->right;
	if (y->right != &t->nil)
		rb_set_parent(y->right, x);
	rb_set_parent(y, RB_PARENT(x));
	if (RB_PARENT(x) == &t->nil) {
		t->root = y;
	} else if (x == RB_PARENT(x)->right) {
		RB_PARENT(x)->right = y;
	} else {
		RB_PARENT(x)->left = y;
	}
	y->right = x;
	rb_set_parent(x, y);
}

void rb_insert_fixup(struct rb_tree *t, struct rb_node *x)
{
	struct rb_node *y;

	rb_set_color(x, RB_RED);
	while ((x != t->root) && (RB_COLOR(RB_PARENT(x)) == RB_RED)) {
		if (RB_PARENT(x) == RB_PARENT_PARENT(x)->left) {
			y = RB_PARENT_PARENT(x)->right;
			if (RB_COLOR(y) == RB_RED) {
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(y, RB_BLACK);
				rb_set_color(RB_PARENT_PARENT(x), RB_RED);
				x = RB_PARENT_PARENT(x);
			} else {
				if (x == RB_PARENT(x)->right) {
					x = RB_PARENT(x);
					rotate_left(t, x);
				}
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(RB_PARENT_PARENT(x), RB_RED);
				rotate_right(t, RB_PARENT_PARENT(x));
			}
		} else {
			y = RB_PARENT_PARENT(x)->left;
			if (RB_COLOR(y) == RB_RED) {
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(y, RB_BLACK);
				rb_set_color(RB_PARENT_PARENT(x), RB_RED);
				x = RB_PARENT_PARENT(x);
			} else {
				if (x == RB_PARENT(x)->left) {
					x = RB_PARENT(x);
					rotate_right(t, x);
				}
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(RB_PARENT_PARENT(x), RB_RED);
				rotate_left(t, RB_PARENT_PARENT(x));
			}
		}
	}
	rb_set_color(t->root, RB_BLACK);
}

struct rb_node *rb_successor(struct rb_tree *tree, struct rb_node *x)
{
	struct rb_node *y;

	y = x->right;
	if (y != &tree->nil) {
		while (y->left != &tree->nil)
			y = y->left;
		return y;
	} else {
		y = RB_PARENT(x);
		while (x == y->right) {
			x = y;
			y = RB_PARENT(y);
		}
		if (y == tree->root)
			return &tree->nil;
		return y;
	}
}

static void delete_fixup(struct rb_tree *t, struct rb_node *x)
{
	struct rb_node *w;

	while ((RB_COLOR(x) == RB_BLACK) && (t->root != x)) {
		if (x == RB_PARENT(x)->left) {
			w = RB_PARENT(x)->right;
			if (RB_COLOR(w) == RB_RED) {
				rb_set_color(w, RB_BLACK);
				rb_set_color(RB_PARENT(x), RB_RED);
				rotate_left(t, RB_PARENT(x));
				w = RB_PARENT(x)->right;
			}
			if ((RB_COLOR(w->right) == RB_BLACK) && (RB_COLOR(w->left) == RB_BLACK) ) {
				rb_set_color(w, RB_RED);
				x = RB_PARENT(x);
			} else {
				if (RB_COLOR(w->right) == RB_BLACK) {
					rb_set_color(w->left, RB_BLACK);
					rb_set_color(w, RB_RED);
					rotate_right(t, w);
					w = RB_PARENT(x)->right;
				}
				rb_set_color(w, RB_COLOR(RB_PARENT(x)));
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(w->right, RB_BLACK);
				rotate_left(t, RB_PARENT(x));
				x = t->root;
			}
		} else {
			w = RB_PARENT(x)->left;
			if (RB_COLOR(w) == RB_RED) {
				rb_set_color(w, RB_BLACK);
				rb_set_color(RB_PARENT(x), RB_RED);
				rotate_right(t, RB_PARENT(x));
				w = RB_PARENT(x)->left;
			}
			if ((RB_COLOR(w->right) == RB_BLACK) && (RB_COLOR(w->left) == RB_BLACK) ) {
				rb_set_color(w, RB_RED);
				x = RB_PARENT(x);
			} else {
				if (RB_COLOR(w->left) == RB_BLACK) {
					rb_set_color(w->right, RB_BLACK);
					rb_set_color(w, RB_RED);
					rotate_left(t, w);
					w = RB_PARENT(x)->left;
				}
				rb_set_color(w, RB_COLOR(RB_PARENT(x)));
				rb_set_color(RB_PARENT(x), RB_BLACK);
				rb_set_color(w->left, RB_BLACK);
				rotate_right(t, RB_PARENT(x));
				x = t->root;
			}
		}
	}
	rb_set_color(x, RB_BLACK);
}

void rb_transplant(struct rb_tree *t, struct rb_node *u, struct rb_node *v)
{
	if (RB_PARENT(u) == &t->nil)
		t->root = v;
	else if (u == RB_PARENT(u)->left)
		RB_PARENT(u)->left = v;
	else
		RB_PARENT(u)->right = v;
	rb_set_parent(v, RB_PARENT(u));
}

struct rb_node *rb_tree_min(struct rb_tree *t, struct rb_node *z)
{
	while (z->left != &t->nil)
		z = z->left;

	return z;
}

void rb_delete(struct rb_tree *t, struct rb_node *z)
{
	struct rb_node *y = z;
	struct rb_node *x;
	int y_org_color = RB_COLOR(y);

	if (z->left == &t->nil) {
		x = z->right;
		rb_transplant(t, z, z->right);
	} else if (z->right == &t->nil) {
		x = z->left;
		rb_transplant(t, z, z->left);
	} else {
		y = rb_tree_min(t, z->right);
		y_org_color = RB_COLOR(y);
		x = y->right;
		if (RB_PARENT(y) == z) {
			rb_set_parent(x, y);
		} else {
			rb_transplant(t, y, y->right);
			y->right = z->right;
			rb_set_parent(y->right, y);
		}
		rb_transplant(t, z, y);
		y->left = z->left;
		rb_set_parent(y->left, y);
		rb_set_color(y, RB_COLOR(z));
	}
	if (y_org_color == RB_BLACK)
		delete_fixup(t, x);
}
