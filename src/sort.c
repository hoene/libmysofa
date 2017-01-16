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
#include "tools.h"

static int entry_compare(const void *p1, const void *p2)
{
	int a1 = ((struct MYSOFA_LOOKUP_ENTRY*)p1)->value;
	int a2 = ((struct MYSOFA_LOOKUP_ENTRY*)p2)->value;

	if(a1 < a2)
		return -1;
	if(a1 > a2)
		return 1;
	return 0;
}

/*
 * http://stackoverflow.com/questions/1024754/how-to-compute-a-3d-morton-number-interleave-the-bits-of-3-ints
 */
static int get3dmorton(double value, double offset, double gain)
{
	int x = (value-offset)*gain;
	x = (x | x << 16) & 0x30000ff;
	x = (x | x << 8) & 0x300f00f;
	x = (x | x << 4) & 0x30c30c3;
	x = (x | x << 2) & 0x9249249;
	return x;
}

static int coordinate2map(struct MYSOFA_LOOKUP *lookup,double *coordinate)
{
	return (get3dmorton(coordinate[0],lookup->c0_min,lookup->c0_gain)<<2) |
			(get3dmorton(coordinate[1],lookup->c1_min,lookup->c1_gain)<<1) |
			get3dmorton(coordinate[2],lookup->c2_min,lookup->c2_gain);
}

struct MYSOFA_LOOKUP* mysofa_sort(struct MYSOFA_HRTF *hrtf)
{
	int i;

	if(!verifyAttribute(hrtf->SourcePosition.attributes,"Type","cartesian"))
		return NULL;

	struct MYSOFA_LOOKUP *lookup = malloc(sizeof(struct MYSOFA_LOOKUP));
	if(!lookup)
		return NULL;
	lookup->sorted = malloc(sizeof(struct MYSOFA_LOOKUP_ENTRY)*hrtf->SourcePosition.elements/3);
	if(!lookup->sorted) {
		free(lookup);
		return NULL;
	}

	lookup->radius_min = DBL_MAX;
	lookup->radius_max = DBL_MIN;
	lookup->c0_min = DBL_MAX;
	lookup->c0_max = DBL_MIN;
	lookup->c1_min = DBL_MAX;
	lookup->c1_max = DBL_MIN;
	lookup->c2_min = DBL_MAX;
	lookup->c2_max = DBL_MIN;
	lookup->elements = hrtf->SourcePosition.elements/3;
	lookup->up = lookup->down = -1;
	lookup->left = lookup->right = -1;
	lookup->front = lookup->back = -1;

	/*
	 * find smallest and largest coordinates
	 */
	for(i=0;i<hrtf->SourcePosition.elements;i+=3) {
		double f=hrtf->SourcePosition.values[i];
		if(f<lookup->c0_min)
			lookup->c0_min=f;
		if(f>lookup->c0_max)
			lookup->c0_max=f;
		f=hrtf->SourcePosition.values[i+1];
		if(f<lookup->c1_min)
			lookup->c1_min=f;
		if(f>lookup->c1_max)
			lookup->c1_max=f;
		f=hrtf->SourcePosition.values[i+2];
		if(f<lookup->c2_min)
			lookup->c2_min=f;
		if(f>lookup->c2_max)
			lookup->c2_max=f;
	}
	if(lookup->c0_max==lookup->c0_min)
		lookup->c0_gain = 0;
	else
		lookup->c0_gain = 1023. / (lookup->c0_max-lookup->c0_min);
	if(lookup->c1_max==lookup->c1_min)
		lookup->c1_gain = 0;
	else
		lookup->c1_gain = 1023. / (lookup->c1_max-lookup->c1_min);
	if(lookup->c2_max==lookup->c2_min)
		lookup->c2_gain = 0;
	else
		lookup->c2_gain = 1023. / (lookup->c2_max-lookup->c2_min);

	/*
	 * find smallest and largest radius
	 */
		for(i=0;i<hrtf->SourcePosition.elements;i+=3) {
			double r = radius(hrtf->SourcePosition.values+i);
			if(r<lookup->radius_min)
				lookup->radius_min=r;
			if(r>lookup->radius_max)
				lookup->radius_max=r;
		}

		/**
	 * make table with 1d coordinate mapping and indices
	 */
	for(i=0;i<lookup->elements;i++) {
		lookup->sorted[i].index=i;
		lookup->sorted[i].value=coordinate2map(lookup,hrtf->SourcePosition.values+i*3);
	}

	qsort(lookup->sorted, lookup->elements, sizeof(struct MYSOFA_LOOKUP_ENTRY), entry_compare);

#if 0
	for(i=0;i<hrtf->SourcePosition.elements/3;i++)
		printf("%f %f %f %f\n", hrtf->SourcePosition.values[lookup->sorted[i].index*3],
				hrtf->SourcePosition.values[lookup->sorted[i].index*3+1],
				hrtf->SourcePosition.values[lookup->sorted[i].index*3+2],
				hrtf->SourcePosition.values[lookup->sorted[i].index*3]+
								hrtf->SourcePosition.values[lookup->sorted[i].index*3+1]+
								hrtf->SourcePosition.values[lookup->sorted[i].index*3+2]
		);
#endif

	return lookup;
}

/*
 * looks for a filter that is similar to the given coordinate
 * BE AwARE: The coordinate vector will be normalized
 */
int mysofa_lookup(struct MYSOFA_HRTF *hrtf, struct MYSOFA_LOOKUP *lookup, double *coordinate)
{
	struct MYSOFA_LOOKUP_ENTRY e;

	double r = radius(coordinate);
	if(r==0)
		return -1;
	if(r > lookup->radius_max) {
		r = lookup->radius_min / r;
		coordinate[0] *= r;
		coordinate[1] *= r;
		coordinate[2] *= r;
	}
	else if(r < lookup->radius_min) {
		r = lookup->radius_max / r;
		coordinate[0] *= r;
		coordinate[1] *= r;
		coordinate[2] *= r;
	}

	e.value = coordinate2map(lookup, coordinate);
	int l,h;
	nsearch(&e, lookup->sorted, lookup->elements, sizeof(e), entry_compare, &l, &h);

	double r1=DBL_MAX,r2=DBL_MAX;
	if(l>=0)
		r1 = distance(coordinate, hrtf->SourcePosition.values+ lookup->sorted[l].index*3);
	if(l!=h && h>=0)
		r2 = distance(coordinate, hrtf->SourcePosition.values + lookup->sorted[h].index*3);
	if(r1 < r2)
		return lookup->sorted[l].index*3;
	else
		return lookup->sorted[h].index*3;
}

void mysofa_lookup_free(struct MYSOFA_LOOKUP *lookup)
{
	free(lookup->sorted);
	free(lookup);
}
