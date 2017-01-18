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
	int i;

	hrtf = mysofa_load("../../tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa",
				&err);
	if (!hrtf)
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

// TODO add some tests...
	double fir[hrtf->N * hrtf->R];
	double delays[2];
	double *res;
	int neighborhood[6] = { -1, -1, -1, -1, -1, -1 };

	res = mysofa_interpolate(hrtf, hrtf->SourcePosition.values, 0, neighborhood,
			fir, delays);
	assert(res == hrtf->DataIR.values);
	assert(delays[0] == 0);
	assert(delays[1] == 0);

	double c[3];
	c[0] = (hrtf->SourcePosition.values[0] + hrtf->SourcePosition.values[3])
			/ 2;
	c[1] = (hrtf->SourcePosition.values[1] + hrtf->SourcePosition.values[4])
			/ 2;
	c[2] = (hrtf->SourcePosition.values[2] + hrtf->SourcePosition.values[5])
			/ 2;
	neighborhood[0] = 3;

	res = mysofa_interpolate(hrtf, c, 0, neighborhood, fir, delays);
	assert(res == fir);
	assert(delays[0] == 0);
	assert(delays[1] == 0);

	for (i = 0; i < hrtf->N * hrtf->R; i++) {
		printf("%f %f %f\n", res[i], hrtf->DataIR.values[i],
				hrtf->DataIR.values[i + hrtf->N * hrtf->R]);
		assert(fequals(res[i],(hrtf->DataIR.values[i]+hrtf->DataIR.values[i+hrtf->N*hrtf->R])/2));
	}

	mysofa_free(hrtf);
	return 0;
}
