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

static void copyArrayWeighted(double *dst, double *src, int size, double w)
{
	while(size>0) {
		*dst++ = *src++ * w;
		size--;
	}
}

static void addArrayWeighted(double *dst, double *src, int size, double w)
{
	while(size>0) {
		*dst++ += *src++ * w;
		size--;
	}
}

static void scaleArray(double *dst, int size, double w)
{
	while(size>0) {
		*dst++ *= w;
		size--;
	}
}

double* mysofa_interpolate(struct MYSOFA_HRTF *hrtf, double *cordinate, int nearest, int *neighborhood, double *fir, double *delays)
{
	int i,use[6];
	double d,d6[6];
	int size = hrtf->N * hrtf->R;

	if(hrtf->DataDelay.elements==hrtf->R) {
		delays[0] = hrtf->DataDelay.values[0];
		delays[1] = hrtf->DataDelay.values[1];
	}

	d=distance(cordinate,hrtf->SourcePosition.values+nearest);
	if(fequals(d,0)) {
		if(hrtf->DataDelay.elements>hrtf->R) {
			delays[0] = hrtf->DataDelay.values[nearest*hrtf->R];
			delays[1] = hrtf->DataDelay.values[nearest*hrtf->R+1];
		}
		return hrtf->DataIR.values + nearest * size;
	}

	for(i=0;i<6;i++)
		use[i]=0;

	if(neighborhood[0]>=0 && neighborhood[1]>=0) {
			d6[0] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[0]);
			d6[1] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[1]);
			if(d6[0]<d6[1])
				use[0]=1;
			else
				use[1]=1;
	}
	else if(neighborhood[0]>=0) {
		d6[0] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[0]);
		use[0]=1;
	}
	else if(neighborhood[1]>=0) {
		d6[1] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[1]);
		use[1]=1;
	}

	if(neighborhood[2]>=0 && neighborhood[3]>=0) {
			d6[2] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[2]);
			d6[3] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[3]);
			if(d6[2]<d6[3])
				use[2]=1;
			else
				use[3]=1;
	}
	else if(neighborhood[2]>=0) {
		d6[2] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[2]);
		use[2]=1;
	}
	else if(neighborhood[3]>=0) {
		d6[3] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[3]);
		use[3]=1;
	}

	if(neighborhood[4]>=0 && neighborhood[5]>=0) {
			d6[4] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[4]);
			d6[5] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[5]);
			if(d6[4]<d6[5])
				use[4]=1;
			else
				use[5]=1;
	}
	else if(neighborhood[4]>=0) {
		d6[4] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[4]);
		use[4]=1;
	}
	else if(neighborhood[5]>=0) {
		d6[5] = distance(cordinate,hrtf->SourcePosition.values+neighborhood[5]);
		use[5]=1;
	}

	double weight = 1/d;
	copyArrayWeighted(fir,hrtf->DataIR.values + nearest*size, size, weight);
	if(hrtf->DataDelay.elements>hrtf->R) {
		delays[0] = hrtf->DataDelay.values[nearest*hrtf->R] * weight;
		delays[1] = hrtf->DataDelay.values[nearest*hrtf->R+1] * weight;
	}



	for(i=0;i<6;i++) {
		if(use[i]) {
			double w = 1/d6[i];
			addArrayWeighted(fir,hrtf->DataIR.values+(neighborhood[i]/3)*size,size,w);
			weight += w;
			if(hrtf->DataDelay.elements>hrtf->R) {
				delays[0] += hrtf->DataDelay.values[nearest*hrtf->R] * w;
				delays[1] += hrtf->DataDelay.values[nearest*hrtf->R+1] * w;
			}
		}
	}

	weight = 1/weight;
	scaleArray(fir,size,weight);
	if(hrtf->DataDelay.elements>hrtf->R) {
		delays[0] *= weight;
		delays[1] *= weight;
	}
	return fir;
}

