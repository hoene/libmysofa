/*
 * loudness.c
 *
 *  Created on: 17.01.2017
 *      Author: hoene
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../resampler/speex_resampler.h"
#include <float.h>
#include <math.h>
#include <assert.h>
#include "mysofa_export.h"
#include "mysofa.h"
#include "tools.h"

MYSOFA_EXPORT float mysofa_loudness(struct MYSOFA_HRTF *hrtf) {
	float c[3], factor;
	float min = FLT_MAX;
	int radius = 0;
	int i, index=0;
	int cartesian = verifyAttribute(hrtf->SourcePosition.attributes, "Type",
					"cartesian");

	/*
	 * find frontal source position
	 */
	for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
		c[0] = hrtf->SourcePosition.values[i];
		c[1] = hrtf->SourcePosition.values[i + 1];
		c[2] = hrtf->SourcePosition.values[i + 2];

		if (cartesian)
			mysofa_c2s(c);

		if (min > c[0] + c[1]) {
			min = c[0] + c[1];
			radius = c[2];
			index = i;
		} else if (min == c[0] + c[1] && radius < c[2]) {
			radius = c[2];
			index = i;
		}
	}

	/* get loudness of frontal fir filter, for both channels*/
	factor = loudness(hrtf->DataIR.values + (index / 3) * hrtf->N * hrtf->R,
			  hrtf->N * hrtf->R);
	factor = sqrt(2 / factor);
	if (fequals(factor, 1.))
		return 1.;

	scaleArray(hrtf->DataIR.values, hrtf->DataIR.elements, factor);

	return factor;
}

