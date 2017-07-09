#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "../hrtf/tools.h"
#include "tests.h"

static void check(char *filename)
{
	struct MYSOFA_HRTF *hrtf;
	int err;

	hrtf = mysofa_load(filename, &err);

	if (!hrtf) {
		CU_FAIL_FATAL("Error reading file.");
		return;
	}

	err = mysofa_check(hrtf);
	CU_ASSERT(err == MYSOFA_OK);

	mysofa_tocartesian(hrtf);

	CU_ASSERT(err == MYSOFA_OK);

	mysofa_free(hrtf);
}

void test_check() {
	check("tests/Pulse.sofa");
	check("share/MIT_KEMAR_normal_pinna.sofa");
}

