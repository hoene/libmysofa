#include <assert.h>
#include "tools.h"

static int cmp(const void *key, const void *elt)
{
	int a1 = *(int*)key;
	int a2 = *(int*)elt;
	if(a1<a2)
		return -1;
	if(a2<a1)
		return 1;
	return 0;
}

int main()
{
	int array[] = { 10, 20, 30, 40, 50 };
	int *res;
	int key;

	assert(cmp(array,array+1)<0);
	assert(cmp(array,array)==0);
	assert(cmp(array+1,array)>0);


	key=10;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array);

	key=20;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+1);

	key=50;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+4);

	key=0;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array);

	key=60;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+4);

	key=11;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+1);

	key=41;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+4);

	key=19;
	res=nsearch(&key, array, 5, sizeof(int), cmp);
	assert(res==array+1);

	return 0;
}
