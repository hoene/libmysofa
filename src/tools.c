/*
 * tools.c
 *
 *  Created on: 13.01.2017
 *      Author: hoene
 */

#define _GNU_SOURCE
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tools.h"

int verifyAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value) {
	while (attr) {
		if (!strcmp(name, attr->name) && !strcmp(value, attr->value))
			return 1;
		attr = attr->next;
	}
	return 0;
}

int changeAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value,
		char *newvalue) {
	while (attr) {
		if (!strcmp(name, attr->name)
				&& (value == NULL || !strcmp(value, attr->value))) {
			free(attr->value);
			attr->value = strdup(newvalue);
			return 1;
		}
		attr = attr->next;
	}
	return 0;
}

void convertCartesianToSpherical(double *values, int elements) {
	int i;
	double x, y, z, r, theta, phi;

	for (i = 0; i < elements - 2; i += 3) {
		x = values[i];
		y = values[i + 1];
		z = values[i + 2];
		r = radius(values);

		theta = atan2(z, sqrt(x * x + y * y));
		phi = atan2(y, x);

		values[i] = fmod(phi * (180 / M_PI) + 360, 360);
		values[i + 1] = theta * (180 / M_PI);
		values[i + 2] = r;
	}
}

void convertSphericalToCartesian(double *values, int elements) {
	int i;
	double x, y, z, r, theta, phi;

	for (i = 0; i < elements - 2; i += 3) {
		phi = values[i] * (M_PI / 180);
		theta = values[i + 1] * (M_PI / 180);
		r = values[i + 2];
		sincos(theta, &z, &x);
		values[i + 2] = z * r;
		x *= r;
		sincos(phi, &y, &z);
		values[i] = z * x;
		values[i + 1] = y * x;
	}
}

double radius(double *cartesian) {
	return sqrt(
			pow(cartesian[0], 2.) + pow(cartesian[1], 2.)
					+ pow(cartesian[2], 2.));
}

double distance(double *cartesian1, double *cartesian2) {
	return sqrt(
			pow(cartesian1[0] - cartesian2[0], 2.)
					+ pow(cartesian1[1] - cartesian2[1], 2.)
					+ pow(cartesian1[2] - cartesian2[2], 2.));
}

/*
 static int fequalsf(float a, float b)
 {
 return fabsf(a-b)<0.000001;
 }
 */

int fequals(double a, double b) {
	return fabs(a - b) < 0.000001;
}

/*
 * search of the nearest
 */

void nsearch(const void *key, const void *base, size_t num, size_t size,
		int (*cmp)(const void *key, const void *elt), int *lower, int *higher) {
	size_t start = 0, end = num;
	int result;

	while (start < end) {
		size_t mid = start + (end - start) / 2;

		result = cmp(key, base + mid * size);
		if (result < 0)
			end = mid;
		else if (result > 0)
			start = mid + 1;
		else {
			*lower = mid;
			*higher = mid;
			return;
		}
	}

	if (start == num) {
		*lower = start - 1;
		*higher = -1;
	} else if (start == 0) {
		*lower = -1;
		*higher = 0;
	} else {
		*lower = start - 1;
		*higher = start;
	}
}

void copyToFloat(float *out, double *in, int size) {
	while (size > 0) {
		*out++ = *in++;
		size--;
	}
}

void copyFromFloat(double *out, float *in, int size) {
	while (size > 0) {
		*out++ = *in++;
		size--;
	}
}

void copyArrayWeighted(double *dst, double *src, int size, double w) {
	while (size > 0) {
		*dst++ = *src++ * w;
		size--;
	}
}

void addArrayWeighted(double *dst, double *src, int size, double w) {
	while (size > 0) {
		*dst++ += *src++ * w;
		size--;
	}
}

void scaleArray(double *dst, int size, double w) {
	while (size > 0) {
		*dst++ *= w;
		size--;
	}
}
double loudness(double *in, int size) {
	double res = 0;
	while (size > 0) {
		res += *in * *in;
		in++;
		size--;
	}
	return res;
}

