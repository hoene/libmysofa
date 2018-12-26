/*
 * tools.h
 *
 *  Created on: 13.01.2017
 *      Author: hoene
 */

#ifndef SRC_TOOLS_H_
#define SRC_TOOLS_H_

#include <stdlib.h>
#include <math.h>
#include "mysofa.h"
#include "mysofa_export.h"

MYSOFA_EXPORT int changeAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value,
		    char *newvalue);
MYSOFA_EXPORT int verifyAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value);
MYSOFA_EXPORT char* getAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name);

MYSOFA_EXPORT void convertCartesianToSpherical(float *values, int elements);
MYSOFA_EXPORT void convertSphericalToCartesian(float *values, int elements);

#define fequals(a,b) (fabsf(a - b) < 0.00001)

MYSOFA_EXPORT float radius(float *cartesian);

#define distance(cartesian1, cartesian2) (sqrtf(powf((cartesian1)[0] - (cartesian2)[0], 2.f) + powf((cartesian1)[1] - (cartesian2)[1], 2.f) + powf((cartesian1)[2] - (cartesian2)[2], 2.f)))

MYSOFA_EXPORT void copyToFloat(float *out, float *in, int size);
MYSOFA_EXPORT void copyFromFloat(float *out, float *in, int size);

MYSOFA_EXPORT void copyArrayWeighted(float *dst, float *src, int size, float w);
MYSOFA_EXPORT void addArrayWeighted(float *dst, float *src, int size, float w);
MYSOFA_EXPORT void scaleArray(float *dst, int size, float w);
MYSOFA_EXPORT float loudness(float *in, int size);

MYSOFA_EXPORT void nsearch(const void *key, const char *base, size_t num, size_t size,
	     int (*cmp)(const void *key, const void *elt), int *lower, int *higher);

#endif /* SRC_TOOLS_H_ */
