/*
 * lib/rbtree.h
 *
 * Copyright (c) 2017 Baruch Marcot
 */

#ifndef LIB_RBTREE_H
#define LIB_RBTREE_H

struct rb_node {
	unsigned long  parent__color;  /* composite field */
	struct rb_node *left;
	struct rb_node *right;
};

struct rb_tree {
	struct rb_node *root;
	struct rb_node nil;
};

#define RB_BLACK  0
#define RB_RED    1

#define RB_TREE(name)  struct rb_tree name = { .root = &(name).nil };

#define RB_PARENT(x)         ((struct rb_node *)((x)->parent__color & ~1ul))
#define RB_PARENT_PARENT(x)  RB_PARENT(RB_PARENT(x))
#define RB_COLOR(x)          ((x)->parent__color & 1)

static inline void INIT_RB_TREE(struct rb_tree *tree)
{
	tree->root = &tree->nil;
}

static inline void INIT_RB_NODE(struct rb_node *x, struct rb_tree *tree)
{
	x->parent__color = ((unsigned long)&tree->nil & ~1ul) | RB_BLACK;
	x->left = x->right = &tree->nil;
}

/* assign a new parent link to the node, while preserving its color */
static inline void rb_set_parent(struct rb_node *x, struct rb_node *parent)
{
	x->parent__color = ((unsigned long)parent & ~1ul) | RB_COLOR(x);
}

/* assign a new color to the node, while preserving its parent link */
static inline void rb_set_color(struct rb_node *x, int color)
{
	x->parent__color = (x->parent__color & ~1ul) | color;
}

/* forward declarations */

struct rb_node *rb_successor(struct rb_tree *tree, struct rb_node *x);
struct rb_node *rb_tree_min(struct rb_tree *t, struct rb_node *z);
void           rb_insert_fixup(struct rb_tree *t, struct rb_node *x);
void           rb_delete(struct rb_tree *t, struct rb_node *z);
void           rb_transplant(struct rb_tree *t, struct rb_node *u,
			struct rb_node *v);

#endif /* !LIB_RBTREE_H */
