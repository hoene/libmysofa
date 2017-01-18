#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "tools.h"

static void copy(double *src, double *dst) {
	memcpy(src, dst, sizeof(double) * 3);
}

static void print(double *c) {
	printf("%f %f %f\n", c[0], c[1], c[2]);
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

	assert(!same(array000, result));
	copy(result, array000);
	assert(same(array000, result));
	print(result);

	convertCartesianToSpherical(result, 3);
	assert(same(array000, result));
	convertSphericalToCartesian(result, 3);
	assert(same(array000, result));

	copy(result, array100);
	convertCartesianToSpherical(result, 3);
	assert(same(array001, result));
	convertSphericalToCartesian(result, 3);
	assert(same(array100, result));
	print(result);

	copy(result, array001);
	convertCartesianToSpherical(result, 3);
	print(result);
	assert(same(array0901, result));
	convertSphericalToCartesian(result, 3);
	assert(same(array001, result));

	copy(result, array010);
	convertCartesianToSpherical(result, 3);
	print(result);
	assert(same(array9001, result));
	convertSphericalToCartesian(result, 3);
	assert(same(array010, result));

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

	assert(cmp(array, array + 1) < 0);
	assert(cmp(array, array) == 0);
	assert(cmp(array + 1, array) > 0);

	key = 10;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 0 && h == 0);

	key = 20;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 1 && h == 1);

	key = 50;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 4 && h == 4);

	key = 0;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l < 0 && h == 0);

	key = 60;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 4 && h < 0);

	key = 11;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 0 && h == 1);

	key = 41;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 3 && h == 4);

	key = 19;
	nsearch(&key, array, 5, sizeof(int), cmp, &l, &h);
	assert(l == 0 && h == 1);
}

int main() {
	test_coordinates();
	test_nsearch();

	return 0;
}
