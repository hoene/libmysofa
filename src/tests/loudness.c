#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "tests.h"

void test_loudness() {
	struct MYSOFA_HRTF *hrtf;
	int err = 0;
	double factor;

	hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
	}

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf("loudness of Pulse.sofa %f\n", factor);
#endif
	CU_ASSERT(fequals(factor,1));
	mysofa_free(hrtf);



		hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa",
			&err);
	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
	}

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf("loudness of MIT_KEMAR_normal_pinna.sofa.sofa %f\n", factor);
#endif
	CU_ASSERT(fequals(factor,1.116589));

	factor = mysofa_loudness(hrtf);
#ifdef VDEBUG
	printf(
			"loudness of MIT_KEMAR_normal_pinna.sofa.sofa after normalization %f\n",
			factor);
#endif
	CU_ASSERT(fequals(factor,1.));

	mysofa_free(hrtf);
}
