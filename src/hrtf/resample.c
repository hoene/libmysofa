/*
 * resample.c
 *
 *  Created on: 17.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libresample.h>
#include <math.h>
#include <assert.h>
#include "mysofa.h"
#include "tools.h"

int mysofa_resample(struct MYSOFA_HRTF *hrtf, float samplerate) {
	int i, res;
	float factor;
	int newN;
    float *values;
    void* handle;
	int used;
	float *in;
	float *out;
    
	if (hrtf->DataSamplingRate.elements != 1 || samplerate < 8000.)
		return MYSOFA_INVALID_FORMAT;

	if (samplerate == hrtf->DataSamplingRate.values[0])
		return MYSOFA_OK;

	factor = samplerate / hrtf->DataSamplingRate.values[0];
	newN = ceil(hrtf->N * factor);

	/*
	 * resample FIR filter
	 */
	values = malloc(newN * hrtf->R * hrtf->M * sizeof(float));
	if (values == NULL)
		return MYSOFA_NO_MEMORY;

	handle = resample_open(1, factor, factor);

    in = malloc(sizeof(float)*hrtf->N);
    out = malloc(sizeof(float)*newN);
    
	for (i = 0; i < hrtf->R * hrtf->M; i++) {
		copyToFloat(in, hrtf->DataIR.values + i * hrtf->N, hrtf->N);
		res = resample_process(handle, factor, in, hrtf->N, 1, &used, out,
				newN);
		assert(res > newN - 8 && res <= newN);
		assert(used == hrtf->N);
		copyFromFloat(values + i * newN, out, res);
		while (res < newN) {
			values[i * newN + res] = 0.;
			res++;
		}
	}
	free(out);
	free(in);
	resample_close(handle);

	free(hrtf->DataIR.values);
	hrtf->DataIR.values = values;
	hrtf->DataIR.elements = newN * hrtf->R * hrtf->M;

	/*
	 * update delay values
	 */
	for (i = 0; i < hrtf->DataIR.elements; i++)
		hrtf->DataIR.values[i] /= factor;

	/*
	 * update sample rate
	 */
	hrtf->DataSamplingRate.values[0] = samplerate;
	hrtf->N = newN;

	return MYSOFA_OK;
}

