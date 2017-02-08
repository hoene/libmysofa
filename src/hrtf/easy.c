/*
 * easy.c
 *
 *  Created on: 18.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "mysofa.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 *
 */

struct MYSOFA_EASY* mysofa_open(const char *filename, float samplerate, int *filterlength, int *err)
{
	fprintf(stderr,"mysofa_open %s %f\n",filename,samplerate);

	struct MYSOFA_EASY *easy = malloc(sizeof(struct MYSOFA_EASY));
	if(!easy) {
		*err = MYSOFA_NO_MEMORY;
		return NULL;
	}
	easy->lookup = NULL;
	easy->neighborhood = NULL;

	easy->hrtf = mysofa_load(filename, err);
	if (!easy->hrtf) {
		fprintf(stderr,"cnanot open");
		mysofa_close(easy);
		return NULL;
	}

	*err = mysofa_check(easy->hrtf);
	if (*err != MYSOFA_OK) {
		fprintf(stderr,"check failed");
		mysofa_close(easy);
		return NULL;
	}

	*err = mysofa_resample(easy->hrtf, samplerate);
	if (*err != MYSOFA_OK) {
		fprintf(stderr,"resample failed");
		mysofa_close(easy);
		return NULL;
	}

	mysofa_loudness(easy->hrtf);

	mysofa_minphase(easy->hrtf,0.01);

	mysofa_tocartesian(easy->hrtf);

	easy->lookup = mysofa_lookup_init(easy->hrtf);
	if (easy->lookup == NULL) {
		*err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(easy);
		return NULL;
	}

	easy->neighborhood = mysofa_neighborhood_init(easy->hrtf,
			easy->lookup);

	return easy;
}

struct MYSOFA_EASY* mysofa_open_cached(const char *filename, float samplerate, int *filterlength, int *err)
{
	struct MYSOFA_EASY* res = mysofa_cache_lookup(filename, samplerate);
	if(res) {
		*filterlength = res->hrtf->N;
		return res;
	}
	res = mysofa_open(filename,samplerate,filterlength,err);
	if(res) {
		res = mysofa_cache_store(res,filename,samplerate);
	}
	return res;
}

void mysofa_getfilter_short(struct MYSOFA_EASY* easy, float x, float y, float z,
		short *IRleft, short *IRright,
		int *delayLeft, int *delayRight)
{
	float c[3] = { x,y,z };
	float fir[easy->hrtf->N * easy->hrtf->R];
	float delays[2];

	int nearest = mysofa_lookup(easy->lookup, c);
	int *neighbors = mysofa_neighborhood(easy->neighborhood, nearest);

	mysofa_interpolate(easy->hrtf, c,
			nearest, neighbors,
			fir, delays);

	*delayLeft  = delays[0] * easy->hrtf->DataSamplingRate.values[0];
	*delayRight = delays[1] * easy->hrtf->DataSamplingRate.values[0];

	float *fl = fir;
	float *fr = fir + easy->hrtf->N;
	int i;
	for(i=easy->hrtf->N;i>0;i--) {
		*IRleft++  = *fl++ * 32767.;
		*IRright++ = *fr++ * 32767.;
	}
}

void mysofa_getfilter_float(struct MYSOFA_EASY* easy, float x, float y, float z,
		float *IRleft, float *IRright,
		float *delayLeft, float *delayRight)
{
	float c[3] = { x,y,z };
	float fir[easy->hrtf->N * easy->hrtf->R];
	float delays[2];

	int nearest = mysofa_lookup(easy->lookup, c);
	int *neighbors = mysofa_neighborhood(easy->neighborhood, nearest);

	mysofa_interpolate(easy->hrtf, c,
			nearest, neighbors,
			fir, delays);

	*delayLeft  = delays[0];
	*delayRight = delays[1];

	float *fl = fir;
	float *fr = fir + easy->hrtf->N;
	int i;
	for(i=easy->hrtf->N;i>0;i--) {
		*IRleft++  = *fl++;
		*IRright++ = *fr++;
	}
}

void mysofa_close(struct MYSOFA_EASY* easy)
{
	if(easy) {
		if(easy->neighborhood)
			mysofa_neighborhood_free(easy->neighborhood);
		if(easy->lookup)
			mysofa_lookup_free(easy->lookup);
		if(easy->hrtf)
			mysofa_free(easy->hrtf);
		free(easy);
	}
}

void mysofa_close_cached(struct MYSOFA_EASY* easy)
{
	mysofa_cache_release(easy);
}
