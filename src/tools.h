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

int changeAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value, char *newvalue);
int verifyAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value);

void convertCartesianToSpherical(double *values, int elements);
void convertSphericalToCartesian(double *values, int elements);
int fequalsf(float a, float b);
int fequals(double a, double b);
double radius(double *cartesian);
double distance(double *cartesian1, double *cartesian2);

void nsearch(const void *key, const void *base, size_t num, size_t size,
               int (*cmp)(const void *key, const void *elt), int *lower, int *higher);

#endif /* SRC_TOOLS_H_ */
