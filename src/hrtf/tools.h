/*
 * tools.h
 *
 *  Created on: 13.01.2017
 *      Author: hoene
 */

#ifndef SRC_TOOLS_H_
#define SRC_TOOLS_H_

#include <stdlib.h>
#include "mysofa.h"

int changeAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value,
		char *newvalue);
int verifyAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value);
char* getAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name);

void convertCartesianToSpherical(float *values, int elements);
void convertSphericalToCartesian(float *values, int elements);
int fequalsf(float a, float b);
int fequals(float a, float b);
float radius(float *cartesian);
float distance(float *cartesian1, float *cartesian2);

void copyToFloat(float *out, float *in, int size);  
void copyFromFloat(float *out, float *in, int size);

void copyArrayWeighted(float *dst, float *src, int size, float w);
void addArrayWeighted(float *dst, float *src, int size, float w);
void scaleArray(float *dst, int size, float w);
float loudness(float *in, int size);

void nsearch(const void *key, const char *base, size_t num, size_t size,
		int (*cmp)(const void *key, const void *elt), int *lower, int *higher);

#endif /* SRC_TOOLS_H_ */
