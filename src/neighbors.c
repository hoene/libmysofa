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

struct MYSOFA_NEIGHBORHOOD *mysofa_neighborhood_init(struct MYSOFA_HRTF *hrtf, struct MYSOFA_LOOKUP *lookup, int neighbors)
{
	int i;

	if(neighbors<=0)
		return NULL;

	struct MYSOFA_NEIGHBORHOOD *neighbor = malloc(sizeof(struct MYSOFA_NEIGHBORHOOD));
	if(!neighbor)
		return NULL;

	neighbor->elements = hrtf->SourcePosition.elements/3;
	neighbor->neighbors = neighbors;
	neighbor->index = malloc(sizeof(int) * neighbor->elements * neighbor->neighbors);
	if(!neighbor->index) {
		free(neighbor);
		return NULL;
	}
	for(i=0;i<neighbor->elements * neighbor->neighbors;i++)
		neighbor->index[i]=-1;

	double origin[3], test[3], *res;
	for(i=0;i<hrtf->SourcePosition.elements;i+=3) {
		memcpy(origin,hrtf->SourcePosition.values+i,sizeof(double)*3);
		convertCartesianToSpherical(origin,3);

		printf("\norigin %f %f %f %d\n",origin[0],origin[1],origin[2],i);
		double phi = 0.5;
		do {
			test[0] = origin[0] + phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+0] = res - hrtf->SourcePosition.values;
				printf("right %d %d %f\n",neighbor->index[i*2+0],i-neighbor->index[i*2+0],phi);
				break;
			}
			phi += 0.5;
		}while(phi <= 45);

		phi = 0.5;
		do {
			test[0] = origin[0] - phi;
			test[1] = origin[1];
			test[2] = origin[2];
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+1] = res - hrtf->SourcePosition.values;
				printf("left %d %d %f\n",neighbor->index[i*2+1],i-neighbor->index[i*2+1],-phi);
				break;
			}
			phi += 0.5;
		}while(phi <= 45);

		double theta = 0.5;
		do {
			test[0] = origin[0];
			test[1] = origin[1] + theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+2] = res - hrtf->SourcePosition.values;
				printf("up %d %d %f\n",neighbor->index[i*2+2],i-neighbor->index[i*2+2],theta);
				break;
			}
			theta += 0.5;
		}while(theta <= 45);

		theta = 0.5;
		do {
			test[0] = origin[0];
			test[1] = origin[1] - theta;
			test[2] = origin[2];
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+3] = res - hrtf->SourcePosition.values;
				printf("down %d %d %f\n",neighbor->index[i*2+3],i-neighbor->index[i*2+3],-theta);
				break;
			}
			theta += 0.5;
		}while(theta <= 45);

		double radius = 0.1, radius2;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] + radius;
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+4] = res - hrtf->SourcePosition.values;
				printf("forward %d %d %f\n",i,i-neighbor->index[i*2+4],radius);
				break;
			}
			radius *= 2;
		}while(radius2 <= lookup->radius_max);

		radius = 0.1;
		do {
			test[0] = origin[0];
			test[1] = origin[1];
			radius2 = test[2] = origin[2] - radius;
			convertSphericalToCartesian(test,3);
			res = mysofa_lookup(lookup,test);
			if(res - hrtf->SourcePosition.values != i) {
				neighbor->index[i*2+5] = res - hrtf->SourcePosition.values;
				printf("backward %d %d %f\n",i,i-neighbor->index[i*2+5],-radius);
				break;
			}
			radius *= 2;
		}while(radius2 >= lookup->radius_min);
	}
	return neighbor;
}

int* mysofa_neighborhood(struct MYSOFA_NEIGHBORHOOD *neighborhood, int pos)
{
	return NULL;
}

void mysofa_neighborhood_free(struct MYSOFA_NEIGHBORHOOD *neighborhood)
{
	free(neighborhood->index);
	free(neighborhood);
}



