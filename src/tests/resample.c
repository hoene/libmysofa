#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "tests.h"

void test_resample() {
	struct MYSOFA_HRTF *hrtf;
	int err = 0, i;
	float *backup;

	hrtf = mysofa_load("tests/Pulse.sofa", &err);
	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
		return;
	}

	backup = malloc(sizeof(float)*hrtf->N * 3);
	if(!backup) {
		CU_FAIL_FATAL("No memory, N is too large.");
		mysofa_free(hrtf);
		return;
	}

	for (i = 0; i < hrtf->N * 3; i++) {
		backup[i]=hrtf->DataIR.values[i];
	}

	err = mysofa_resample(hrtf, 96000.);
	CU_ASSERT_FATAL(err == MYSOFA_OK);

	for (i = 0; i < hrtf->N * 3; i++) {
#ifdef VDEBUG
		printf("%6.3f~%6.3f ", hrtf->DataIR.values[i], backup[i/2]);
		if ((i % hrtf->N) == (hrtf->N - 1))
			printf("\n");
#endif
		CU_ASSERT( !((hrtf->DataIR.values[i]>0.4 && backup[i/2]==0.) ||
			     (hrtf->DataIR.values[i]<=0.3 && backup[i/2]==1.) ) );
	}

	free(backup);
	mysofa_free(hrtf);
}

