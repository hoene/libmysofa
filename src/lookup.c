/*
 * sort.c
 *
 *  Created on: 12.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "mysofa.h"
#include "kdtree.h"
#include "tools.h"

struct MYSOFA_LOOKUP* mysofa_lookup_init(struct MYSOFA_HRTF *hrtf) {
	int i;

	/*
	 * alloc memory structure
	 */
	if (!verifyAttribute(hrtf->SourcePosition.attributes, "Type", "cartesian"))
		return NULL ;

	struct MYSOFA_LOOKUP *lookup = malloc(sizeof(struct MYSOFA_LOOKUP));
	if (!lookup)
		return NULL ;

	/*
	 * find smallest and largest radius
	 */
	lookup->radius_min = DBL_MAX;
	lookup->radius_max = DBL_MIN;
	for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
		double r = radius(hrtf->SourcePosition.values + i);
		if (r < lookup->radius_min)
			lookup->radius_min = r;
		if (r > lookup->radius_max)
			lookup->radius_max = r;
	}

	/*
	 * Allocate kd tree
	 */
	lookup->kdtree = kd_create(3);
	if (!lookup->kdtree) {
		free(lookup);
		return NULL ;
	}

	/*
	 * add coordinates to the tree
	 */
	for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
		double *f = hrtf->SourcePosition.values + i;
		kd_insert((struct kdtree *) lookup->kdtree, f, f);
	}

	return lookup;
}

/*
 * looks for a filter that is similar to the given coordinate
 * BE AWARE: The coordinate vector will be normalized if needed
 */
double* mysofa_lookup(struct MYSOFA_LOOKUP *lookup, double *coordinate) {

    double r = radius(coordinate);
    if(r>lookup->radius_max) {
            r = lookup->radius_max / r;
            coordinate[0] *= r;
            coordinate[1] *= r;
            coordinate[2] *= r;
    }
    else if(r<lookup->radius_min) {
            r = lookup->radius_min / r;
            coordinate[0] *= r;
            coordinate[1] *= r;
            coordinate[2] *= r;
    }

	struct kdres *res = kd_nearest((struct kdtree *) lookup->kdtree,
			coordinate);
	if (kd_res_size(res) != 1) {
		kd_res_free(res);
		return NULL ;
	}
	coordinate = (double*) kd_res_item_data(res);
	kd_res_free(res);
	return coordinate;
}

void mysofa_lookup_free(struct MYSOFA_LOOKUP *lookup) {
	if(lookup) {
		kd_free((struct kdtree *) lookup->kdtree);
		free(lookup);
	}
}
