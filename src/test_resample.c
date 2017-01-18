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
	struct MYSOFA_HRTF *hrtf;
	int err = 0, i;

	hrtf = mysofa_load("../../tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf)
		hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n", err);
		return err;
	}

	double backup[hrtf->N*3];

	for (i = 0; i < hrtf->N * 3; i++) {
		backup[i]=hrtf->DataIR.values[i];
	}

	err = mysofa_resample(hrtf, 96000.);
	if (err != MYSOFA_OK) {
		fprintf(stderr, "Error resampling HRTF. Error code: %d\n", err);
		return err;
	}

	for (i = 0; i < hrtf->N * 3; i++) {
		printf("%6.3f~%6.3f ", hrtf->DataIR.values[i], backup[i/2]);
		if ((i % hrtf->N) == (hrtf->N - 1))
			printf("\n");
		assert( !((hrtf->DataIR.values[i]>0.4 && backup[i/2]==0.) ||
				(hrtf->DataIR.values[i]<=0.3 && backup[i/2]==1.) ) );
	}

	mysofa_free(hrtf);
	return 0;
}
