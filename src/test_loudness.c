#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "mysofa.h"
#include "tools.h"


int main()
{
	struct MYSOFA_HRTF *hrtf;
	int err = 0;
	double factor;

	hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}

    factor=mysofa_loudness(hrtf);
    printf("loudness of Pulse.sofa %f\n",factor);
    assert(fequals(factor,1));
    mysofa_free(hrtf);

	hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}

    factor=mysofa_loudness(hrtf);
    printf("loudness of MIT_KEMAR_normal_pinna.sofa.sofa %f\n",factor);
    assert(fequals(factor,1.116589));

    factor=mysofa_loudness(hrtf);
    printf("loudness of MIT_KEMAR_normal_pinna.sofa.sofa after normalization %f\n",factor);
    assert(fequals(factor,1.));

    mysofa_free(hrtf);

	return 0;
}
