/*
 * neighbor.c
 *
 *  Created on: 17.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mysofa.h"
#include "tools.h"

struct MYSOFA_NEIGHBORHOOD *mysofa_neighborhood_init(struct MYSOFA_HRTF *hrtf,
		struct MYSOFA_LOOKUP *lookup) {
	int i;

	struct MYSOFA_NEIGHBORHOOD *neighbor = malloc(
			sizeof(struct MYSOFA_NEIGHBORHOOD));
	if (!neighbor)
		return NULL ;

	neighbor->elements = hrtf->SourcePosition.elements / 3;
	neighbor->index = malloc(sizeof(int) * neighbor->elements * 6);
	if (!neighbor->index) {
		free(neighbor);
		return NULL ;
	}
	for (i = 0; i < neighbor->elements * 6; i++)
		neighbor->index[i] = -1;

	double origin[3], test[3], *res;
	for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
		memcpy(origin, hrtf->SourcePosition.values + i, sizeof(double) * 3);
		convertCartesianToSpherical(origin, 3);

		double phi = 0.5;
		do {
			test[0] = origin[0] + phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 0] = res - hrtf->SourcePosition.values;
				break;
			}
			phi += 0.5;
		} while (phi <= 45);

		phi = 0.5;
		do {
			test[0] = origin[0] - phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 1] = res - hrtf->SourcePosition.values;
				break;
			}
			phi += 0.5;
		} while (phi <= 45);

		double theta = 0.5;
		do {
			test[0] = origin[0];
			test[1] = origin[1] + theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 2] = res - hrtf->SourcePosition.values;
				break;
			}
			theta += 0.5;
		} while (theta <= 45);

		theta = 0.5;
		do {
			test[0] = origin[0];
			test[1] = origin[1] - theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 3] = res - hrtf->SourcePosition.values;
				break;
			}
			theta += 0.5;
		} while (theta <= 45);

		double radius = 0.1, radius2;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] + radius;
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 4] = res - hrtf->SourcePosition.values;
				break;
			}
			radius *= 2;
		} while (radius2 <= lookup->radius_max);

		radius = 0.1;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] - radius;
			convertSphericalToCartesian(test, 3);
			res = mysofa_lookup(lookup, test);
			if (res - hrtf->SourcePosition.values != i) {
				neighbor->index[i * 2 + 5] = res - hrtf->SourcePosition.values;
				break;
			}
			radius *= 2;
		} while (radius2 >= lookup->radius_min);
	}
	return neighbor;
}

int* mysofa_neighborhood(struct MYSOFA_NEIGHBORHOOD *neighborhood, int pos) {
	if (pos < 0 || pos >= neighborhood->elements * 3)
		return NULL ;
	return neighborhood->index + pos * 2;
}

void mysofa_neighborhood_free(struct MYSOFA_NEIGHBORHOOD *neighborhood) {
	free(neighborhood->index);
	free(neighborhood);
}

