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
	int len, i;

	hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}
    len=mysofa_minphase(hrtf,0.01);
    printf("max length %d\n",len);
    assert(len==1);
    for(i=0;i<hrtf->DataIR.elements;i++)
    	assert(fequals(hrtf->DataIR.values[i],1));
    mysofa_free(hrtf);

	hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}

    len=mysofa_minphase(hrtf,0.01);
    printf("max length %d\n",len);
    assert(len==361);
    mysofa_free(hrtf);

	hrtf = mysofa_load("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa", &err);
	if (!hrtf) {
		fprintf(stderr, "Error reading file. Error code: %d\n",
				err);
		return err;
	}

    len=mysofa_minphase(hrtf,0.001);
    printf("max length %d\n",len);
    assert(len==463);
    mysofa_free(hrtf);

	return 0;
}
