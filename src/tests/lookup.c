#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../hrtf/tools.h"
#include "tests.h"

void test_lookup() {
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;
	struct timeval r1, r2;
	double duration1, duration2;

	hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);

	if (!hrtf)
		CU_FAIL_FATAL("Error reading file.");

	mysofa_tocartesian(hrtf);

	struct MYSOFA_LOOKUP *lookup = mysofa_lookup_init(hrtf);
	if (lookup == NULL) {
		CU_FAIL("Error sorting HRTF.");
		mysofa_free(hrtf);
		return;
	}

#ifdef VDEBUG
	printf("r  %f %f\n", lookup->radius_min, lookup->radius_max);
#endif

	double find[3];
	int j;
	for (j = 0; j < 10000; j++) {
		find[0] = rand() * (4. / RAND_MAX) - 2;
		find[1] = rand() * (4. / RAND_MAX) - 2;
		find[2] = rand() * (4. / RAND_MAX) - 2;

		gettimeofday(&r1, NULL);
		int lk = mysofa_lookup(lookup, find);
		gettimeofday(&r2, NULL);
		duration1 = (r2.tv_sec - r1.tv_sec) * 1000000.
				+ (r2.tv_usec - r1.tv_usec);

		gettimeofday(&r1, NULL);
		int index = -1;
		double dmin = DBL_MAX;
		int i;
		for (i = 0; i < hrtf->M; i ++) {
			double r = distance(find, hrtf->SourcePosition.values + i * hrtf->C);
			if (r < dmin) {
				dmin = r;
				index = i;
			}
		}
		gettimeofday(&r2, NULL);
		duration2 = (r2.tv_sec - r1.tv_sec) * 1000000.
				+ (r2.tv_usec - r1.tv_usec);

		CU_ASSERT(lk==index);
		if(lk!=index) {
			printf("O(log n) %f %f %f -> %d %f \t\t", find[0], find[1], find[2], lk,
				distance(find, hrtf->SourcePosition.values + lk  * hrtf->C));
			printf("O(n): %f %f %f -> %d %f\t%f%%\n", find[0], find[1], find[2],
				index, dmin, duration1 / duration2 * 100);
		}
	}

	mysofa_lookup_free(lookup);
	mysofa_free(hrtf);
}

