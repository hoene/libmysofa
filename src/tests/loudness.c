#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "tests.h"

void test_loudness() {
	struct MYSOFA_HRTF *hrtf;
	int err = 0;
	float factor;

	hrtf = mysofa_load("tests/Pulse.sofa", &err);
	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
		return;
	}

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf("loudness of Pulse.sofa %f\n", factor);
#endif
	CU_ASSERT(fequals(factor,1));
	mysofa_free(hrtf);



	hrtf = mysofa_load("share/MIT_KEMAR_normal_pinna.sofa",
			   &err);
	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
	}

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf("loudness of MIT_KEMAR_normal_pinna.sofa %f\n", factor);
#endif
	CU_ASSERT(fequals(factor,1.116589));

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf(
		"loudness of MIT_KEMAR_normal_pinna.sofa after normalization %f\n",
		factor);
#endif
	CU_ASSERT(fequals(factor,1.));

	mysofa_free(hrtf);
}
