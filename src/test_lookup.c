#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "tools.h"

int main() {
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;
	struct timeval r1, r2;
	double duration1, duration2;

	hrtf = mysofa_load("../../tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf)
		hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);

	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n", err);
		return err;
	}

	err = mysofa_check(hrtf);
	if (err != MYSOFA_OK) {
		fprintf(stderr, "Error verifying HRTF. Error code: %d\n", err);
		return err;
	}

	mysofa_tocartesian(hrtf);

	err = mysofa_check(hrtf);
	if (err != MYSOFA_OK) {
		fprintf(stderr,
				"Error verifying HRTF after ToCartesian. Error code: %d\n",
				err);
		return err;
	}

	struct MYSOFA_LOOKUP *lookup = mysofa_lookup_init(hrtf);
	if (lookup == NULL) {
		fprintf(stderr, "Error sorting HRTF.");
		return MYSOFA_INTERNAL_ERROR;
	}

	printf("r  %f %f\n", lookup->radius_min, lookup->radius_max);

	double find[3];
	int j;
	for (j = 0; j < 10000; j++) {
		find[0] = rand() * (4. / RAND_MAX) - 2;
		find[1] = rand() * (4. / RAND_MAX) - 2;
		find[2] = rand() * (4. / RAND_MAX) - 2;

		gettimeofday(&r1, NULL);
		int lk = mysofa_lookup(lookup, find) - hrtf->SourcePosition.values;
		gettimeofday(&r2, NULL);
		duration1 = (r2.tv_sec - r1.tv_sec) * 1000000.
				+ (r2.tv_usec - r1.tv_usec);

		gettimeofday(&r1, NULL);
		int index = -1;
		double dmin = DBL_MAX;
		int i;
		for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
			double r = distance(find, hrtf->SourcePosition.values + i);
			if (r < dmin) {
				dmin = r;
				index = i;
			}
		}
		gettimeofday(&r2, NULL);
		duration2 = (r2.tv_sec - r1.tv_sec) * 1000000.
				+ (r2.tv_usec - r1.tv_usec);

		printf("O(log n) %f %f %f -> %d %f \t\t", find[0], find[1], find[2], lk,
				distance(find, hrtf->SourcePosition.values + lk));
		printf("O(n): %f %f %f -> %d %f\t%f%%\n", find[0], find[1], find[2],
				index, dmin, duration1 / duration2 * 100);
		assert(lk==index);
	}

	mysofa_lookup_free(lookup);
	mysofa_free(hrtf);
	return 0;
}
