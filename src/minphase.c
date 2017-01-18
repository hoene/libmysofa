/*
 * loudness.c
 *
 *  Created on: 17.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <assert.h>
#include "mysofa.h"
#include "tools.h"

static void trunk(double *in, int size, int *start, int *end, double threshold) {
	double energy = 0;
	int s = 0;
	int e = size - 1;
	double ss, ee;

	double l = loudness(in, size);
	threshold = threshold * l;

	ss = in[s] * in[s];
	ee = in[e] * in[e];
	while (s < e) {
		if (ss <= ee) {
			if (energy + ss > threshold)
				break;
			energy += ss;
			s++;
			ss = in[s] * in[s];
		} else {
			if (energy + ee > threshold)
				break;
			energy += ee;
			e--;
			ee = in[e] * in[e];
		}
	}
	*start = s;
	*end = e + 1;
}

int mysofa_minphase(struct MYSOFA_HRTF *hrtf, double threshold) {
	int i;
	int max = 0;

	if (hrtf->DataDelay.elements != 2)
		return -1;

	int filters = hrtf->M * hrtf->R;
	int start[filters];
	int end[filters];

	/*
	 * find maximal length of a filter
	 */
	for (i = 0; i < filters; i++) {
		trunk(hrtf->DataIR.values + i * hrtf->N, hrtf->N, start + i, end + i,
				threshold);
		if (end[i] - start[i] > max)
			max = end[i] - start[i];
	}

	if (max == hrtf->N)
		return max;

	/*
	 * update delay and filters
	 */
	double samplerate = hrtf->DataSamplingRate.values[0];
	double d[2] = { hrtf->DataDelay.values[0], hrtf->DataDelay.values[1] };
	hrtf->DataDelay.elements = filters;
	hrtf->DataDelay.values = realloc(hrtf->DataDelay.values,
			sizeof(double) * filters);
	for (i = 0; i < filters; i++) {
		if (start[i] + max > hrtf->N)
			start[i] = hrtf->N - max;
		hrtf->DataDelay.values[i] = d[i % 1] + (start[i] / samplerate);
		memmove(hrtf->DataIR.values + i * max,
				hrtf->DataIR.values + i * hrtf->N + start[i],
				max * sizeof(double));
	}

	/*
	 * update hrtf structure
	 */
	hrtf->N = max;
	hrtf->DataIR.elements = max * filters;
	hrtf->DataIR.values = realloc(hrtf->DataIR.values,
			sizeof(double) * hrtf->DataIR.elements);

	return max;
}

