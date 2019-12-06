/*
 This file is part of ``kdtree'', a library for working with kd-trees.
 Copyright (C) 2007-2011 John Tsiombikas <nuclear@member.fsf.org>

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 OF SUCH DAMAGE.
 */
/* single nearest neighbor search written by Tamas Nepusz <tamas@cs.rhul.ac.uk> */
#include "kdtree.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct kdhyperrect {
	int dim;
	float *min, *max; /* minimum/maximum coords */
};

struct kdnode {
	float *pos;
	int dir;
	void *data;

	struct kdnode *left, *right; /* negative/positive side */
};

struct kdtree {
	int dim;
	struct kdnode *root;
	struct kdhyperrect *rect;
	void (*destr)(void*);
};

#define SQ(x)			((x) * (x))

static void clear_rec(struct kdnode *node, void (*destr)(void*));
static int insert_rec(struct kdnode **node, const float *pos, void *data,
		int dir, int dim);

static struct kdhyperrect* hyperrect_create(int dim, const float *min,
		const float *max);
static void hyperrect_free(struct kdhyperrect *rect);
static struct kdhyperrect* hyperrect_duplicate(const struct kdhyperrect *rect);
static void hyperrect_extend(struct kdhyperrect *rect, const float *pos);
static float hyperrect_dist_sq(struct kdhyperrect *rect, const float *pos);

struct kdtree *kd_create(int k) {
	struct kdtree *tree;

	if (!(tree = malloc(sizeof *tree))) {
		return 0;
	}

	tree->dim = k;
	tree->root = 0;
	tree->destr = 0;
	tree->rect = 0;

	return tree;
}

static void clear_rec(struct kdnode *node, void (*destr)(void*)) {
	if (!node)
		return;

	clear_rec(node->left, destr);
	clear_rec(node->right, destr);

	if (destr) {
		destr(node->data);
	}
	free(node->pos);
	free(node);
}

static void kd_clear(struct kdtree *tree) {
	clear_rec(tree->root, tree->destr);
	tree->root = 0;

	if (tree->rect) {
		hyperrect_free(tree->rect);
		tree->rect = 0;
	}
}

void kd_free(struct kdtree *tree) {
	if (tree) {
		kd_clear(tree);
		free(tree);
	}
}

static int insert_rec(struct kdnode **nptr, const float *pos, void *data,
		int dir, int dim) {
	int new_dir;
	struct kdnode *node;

	if (!*nptr) {
		if (!(node = malloc(sizeof *node))) {
			return -1;
		}
		if (!(node->pos = malloc(dim * sizeof *node->pos))) {
			free(node);
			return -1;
		}
		memcpy(node->pos, pos, dim * sizeof *node->pos);
		node->data = data;
		node->dir = dir;
		node->left = node->right = 0;
		*nptr = node;
		return 0;
	}

	node = *nptr;
	new_dir = (node->dir + 1) % dim;
	if (pos[node->dir] < node->pos[node->dir]) {
		return insert_rec(&(*nptr)->left, pos, data, new_dir, dim);
	}
	return insert_rec(&(*nptr)->right, pos, data, new_dir, dim);
}

int kd_insert(struct kdtree *tree, const float *pos, void *data) {
	if (insert_rec(&tree->root, pos, data, 0, tree->dim)) {
		return -1;
	}

	if (tree->rect == 0) {
		tree->rect = hyperrect_create(tree->dim, pos, pos);
	} else {
		hyperrect_extend(tree->rect, pos);
	}

	return 0;
}

static void kd_nearest_i(struct kdnode *node, const float *pos,
		struct kdnode **result, float *result_dist_sq, struct kdhyperrect* rect) {
	int dir = node->dir;
	int i;
	float dummy, dist_sq;
	struct kdnode *nearer_subtree, *farther_subtree;
	float *nearer_hyperrect_coord, *farther_hyperrect_coord;

	/* Decide whether to go left or right in the tree */
	dummy = pos[dir] - node->pos[dir];
	if (dummy <= 0) {
		nearer_subtree = node->left;
		farther_subtree = node->right;
		nearer_hyperrect_coord = rect->max + dir;
		farther_hyperrect_coord = rect->min + dir;
	} else {
		nearer_subtree = node->right;
		farther_subtree = node->left;
		nearer_hyperrect_coord = rect->min + dir;
		farther_hyperrect_coord = rect->max + dir;
	}

	if (nearer_subtree) {
		/* Slice the hyperrect to get the hyperrect of the nearer subtree */
		dummy = *nearer_hyperrect_coord;
		*nearer_hyperrect_coord = node->pos[dir];
		/* Recurse down into nearer subtree */
		kd_nearest_i(nearer_subtree, pos, result, result_dist_sq, rect);
		/* Undo the slice */
		*nearer_hyperrect_coord = dummy;
	}

	/* Check the distance of the point at the current node, compare it
	 * with our best so far */
	dist_sq = 0;
	for (i = 0; i < rect->dim; i++) {
		dist_sq += SQ(node->pos[i] - pos[i]);
	}
	if (dist_sq < *result_dist_sq) {
		*result = node;
		*result_dist_sq = dist_sq;
	}

	if (farther_subtree) {
		/* Get the hyperrect of the farther subtree */
		dummy = *farther_hyperrect_coord;
		*farther_hyperrect_coord = node->pos[dir];
		/* Check if we have to recurse down by calculating the closest
		 * point of the hyperrect and see if it's closer than our
		 * minimum distance in result_dist_sq. */
		if (hyperrect_dist_sq(rect, pos) < *result_dist_sq) {
			/* Recurse down into farther subtree */
			kd_nearest_i(farther_subtree, pos, result, result_dist_sq, rect);
		}
		/* Undo the slice on the hyperrect */
		*farther_hyperrect_coord = dummy;
	}
}

int kd_nearest(struct kdtree *kd, const float *pos, void **res) {
	struct kdhyperrect *rect;
	struct kdnode *result;
	float dist_sq;
	int i;

	if (!kd)
		return -1;
	if (!kd->rect)
		return -1;

	/* Duplicate the bounding hyperrectangle, we will work on the copy */
	if (!(rect = hyperrect_duplicate(kd->rect))) {
		return -1;
	}

	/* Our first guesstimate is the root node */
	result = kd->root;
	dist_sq = 0;
	for (i = 0; i < kd->dim; i++)
		dist_sq += SQ(result->pos[i] - pos[i]);

	/* Search for the nearest neighbour recursively */
	kd_nearest_i(kd->root, pos, &result, &dist_sq, rect);

	/* Free the copy of the hyperrect */
	hyperrect_free(rect);

	if (result) {
		*res = result->data;
		return 0;
	} else {
		return -1;
	}
}

/* ---- hyperrectangle helpers ---- */
static struct kdhyperrect* hyperrect_create(int dim, const float *min,
		const float *max) {
	size_t size = dim * sizeof(float);
	struct kdhyperrect* rect = 0;

	if (!(rect = malloc(sizeof(struct kdhyperrect)))) {
		return 0;
	}

	rect->dim = dim;
	if (!(rect->min = malloc(size))) {
		free(rect);
		return 0;
	}
	if (!(rect->max = malloc(size))) {
		free(rect->min);
		free(rect);
		return 0;
	}
	memcpy(rect->min, min, size);
	memcpy(rect->max, max, size);

	return rect;
}

static void hyperrect_free(struct kdhyperrect *rect) {
	free(rect->min);
	free(rect->max);
	free(rect);
}

static struct kdhyperrect* hyperrect_duplicate(const struct kdhyperrect *rect) {
	return hyperrect_create(rect->dim, rect->min, rect->max);
}

static void hyperrect_extend(struct kdhyperrect *rect, const float *pos) {
	int i;

	for (i = 0; i < rect->dim; i++) {
		if (pos[i] < rect->min[i]) {
			rect->min[i] = pos[i];
		}
		if (pos[i] > rect->max[i]) {
			rect->max[i] = pos[i];
		}
	}
}

static float hyperrect_dist_sq(struct kdhyperrect *rect, const float *pos) {
	int i;
	float result = 0;

	for (i = 0; i < rect->dim; i++) {
		if (pos[i] < rect->min[i]) {
			result += SQ(rect->min[i] - pos[i]);
		} else if (pos[i] > rect->max[i]) {
			result += SQ(rect->max[i] - pos[i]);
		}
	}

	return result;
}
