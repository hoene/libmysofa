#include <string.h>
#include <stdio.h>
#include "../hrtf/tools.h"
#include "tests.h"

static void copy(double *src, double *dst) {
	memcpy(src, dst, sizeof(double) * 3);
}

static void print(double *c) {
#ifdef VDEBUG
	printf("%f %f %f\n", c[0], c[1], c[2]);
#endif
}

static int same(double *a, double *b) {
	return fequals(a[0], b[0]) && fequals(a[1], b[1]) && fequals(a[2], b[2]);
}

void test_coordinates() {
	double array000[3] = { 0, 0, 0 };
	double array100[3] = { 1, 0, 0 };
	double array010[3] = { 0, 1, 0 };
	double array001[3] = { 0, 0, 1 };
	double array0901[3] = { 0, 90, 1 };
	double array9001[3] = { 90, 0, 1 };
	double result[3] = { 1, 1, 1 };

	CU_ASSERT(!same(array000, result));
	copy(result, array000);
	CU_ASSERT(same(array000, result));
	print(result);

	convertCartesianToSpherical(result, 3);
	CU_ASSERT(same(array000, result));
	convertSphericalToCartesian(result, 3);
	CU_ASSERT(same(array000, result));

	copy(result, array100);
	convertCartesianToSpherical(result, 3);
	CU_ASSERT(same(array001, result));
	convertSphericalToCartesian(result, 3);
	CU_ASSERT(same(array100, result));
	print(result);

	copy(result, array001);
	convertCartesianToSpherical(result, 3);
	print(result);
	CU_ASSERT(same(array0901, result));
	convertSphericalToCartesian(result, 3);
	CU_ASSERT(same(array001, result));

	copy(result, array010);
	convertCartesianToSpherical(result, 3);
	print(result);
	CU_ASSERT(same(array9001, result));
	convertSphericalToCartesian(result, 3);
	CU_ASSERT(same(array010, result));

}

static int cmp(const void *key, const void *elt) {
	int a1 = *(int*) key;
	int a2 = *(int*) elt;
	if (a1 < a2)
		return -1;
	if (a2 < a1)
		return 1;
	return 0;
}

void test_nsearch() {
	int array[] = { 10, 20, 30, 40, 50 };
	int l, h;
	int key;

	CU_ASSERT(cmp(array, array + 1) < 0);
	CU_ASSERT(cmp(array, array) == 0);
	CU_ASSERT(cmp(array + 1, array) > 0);

	key = 10;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 0 && h == 0);

	key = 20;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 1 && h == 1);

	key = 50;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 4 && h == 4);

	key = 0;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l < 0 && h == 0);

	key = 60;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 4 && h < 0);

	key = 11;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 0 && h == 1);

	key = 41;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 3 && h == 4);

	key = 19;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	CU_ASSERT(l == 0 && h == 1);
}
