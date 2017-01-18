#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "mysofa.h"
#include "tools.h"

int main() {
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;

	hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa",
			&err);

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

	struct MYSOFA_LOOKUP *lookup = mysofa_lookup_init(hrtf);
	if (lookup == NULL) {
		fprintf(stderr, "Error sorting HRTF.");
		return MYSOFA_INTERNAL_ERROR;
	}

	struct MYSOFA_NEIGHBORHOOD *neighborhood = mysofa_neighborhood_init(hrtf,
			lookup);
	int i, j, *res;
	double c[3];
	const char *dir = "RLUDFB";

	for (i = 0; i < hrtf->SourcePosition.elements; i += 3) {
		memcpy(c, hrtf->SourcePosition.values + i, sizeof(double) * 3);
		convertCartesianToSpherical(c, 3);
		printf("%4.0f %4.0f %5.2f\t", c[0], c[1], c[2]);

		res = mysofa_neighborhood(neighborhood, i);
		for (j = 0; j < 6; j++) {
			if (res[j] >= 0) {
				memcpy(c, hrtf->SourcePosition.values + res[j],
						sizeof(double) * 3);
				convertCartesianToSpherical(c, 3);
				printf("\t%c %4.0f %4.0f %5.2f", dir[j], c[0], c[1], c[2]);
			}
		}
		printf("\n");
#if 0

		memcpy(test,res,sizeof(double)*3);
		convertCartesianToSpherical(test,3);
		printf("right %3d\t%f %f %f %f\n",res - hrtf->SourcePosition.values,test[0],test[1],test[2],phi);
#endif
	}

	mysofa_neighborhood_free(neighborhood);

	mysofa_lookup_free(lookup);
	mysofa_free(hrtf);
	return 0;
}
