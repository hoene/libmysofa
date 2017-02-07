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

	if (hrtf->DataSamplingRate.elements != 1 || samplerate < 8000.)
		return MYSOFA_INVALID_FORMAT;

	if (samplerate == hrtf->DataSamplingRate.values[0])
		return MYSOFA_OK;

	float factor = samplerate / hrtf->DataSamplingRate.values[0];
	int newN = ceil(hrtf->N * factor);

	/*
	 * resample FIR filter
	 */
	float *values = malloc(newN * hrtf->R * hrtf->M * sizeof(float));
	if (values == NULL)
		return MYSOFA_NO_MEMORY;

	void* handle = resample_open(1, factor, factor);

	for (i = 0; i < hrtf->R * hrtf->M; i++) {
		int used;
		float in[hrtf->N];
		float out[newN];

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

