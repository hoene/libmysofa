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
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;
	struct timeval r1, r2;
	double duration1, duration2;

	hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa", &err);

	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}

    err=mysofa_check(hrtf);
    if(err!=MYSOFA_OK) {
		fprintf(stderr, "Error verifying HRTF. Error code: %d\n",
				err);
		return err;
    }

    mysofa_tocartesian(hrtf);

    struct MYSOFA_LOOKUP *lookup = mysofa_lookup_init(hrtf);
    if(lookup==NULL) {
		fprintf(stderr, "Error sorting HRTF.");
		return MYSOFA_INTERNAL_ERROR;
    }

    struct MYSOFA_NEIGHBORHOOD *neighborhood = mysofa_neighborhood_init(hrtf,lookup,6);
    mysofa_neighborhood_free(neighborhood);

    mysofa_lookup_free(lookup);
    mysofa_free(hrtf);
	return 0;
}
