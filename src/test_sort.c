#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include "tools.h"


int main()
{
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;

	hrtf = mysofa_load("tests/sofa_api_mo_test/Pulse.sofa", &err);

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

    err=mysofa_check(hrtf);
    if(err!=MYSOFA_OK) {
		fprintf(stderr, "Error verifying HRTF after ToCartesian. Error code: %d\n",
				err);
		return err;
    }

    struct MYSOFA_LOOKUP *lookup = mysofa_sort(hrtf);
    if(lookup==NULL) {
		fprintf(stderr, "Error sorting HRTF.");
		return MYSOFA_INTERNAL_ERROR;
    }

    printf("c0 %f %f\n",lookup->c0_min,lookup->c0_max);
    printf("c1 %f %f\n",lookup->c1_min,lookup->c1_max);
    printf("c2 %f %f\n",lookup->c2_min,lookup->c2_max);
    printf("r  %f %f\n",lookup->radius_min,lookup->radius_max);

    double find[3];
    int j;
    for(j=0;j<10;j++) {
    	find[0] = rand() * (4. / RAND_MAX) - 2;
    	find[1] = rand() * (4. / RAND_MAX) - 2;
    	find[2] = rand() * (4. / RAND_MAX) - 2;

    	int index = -1;
    	double dmin = DBL_MAX;
    	int i;
		for(i=0;i<hrtf->SourcePosition.elements;i+=3) {
			double r = distance(find,hrtf->SourcePosition.values+i);
			if(r<dmin) {
				dmin=r;
				index = i;
			}
		}
		printf("O(n): %f %f %f -> %d %f\n",find[0],find[1],find[2],index,dmin);

		index=mysofa_lookup(hrtf, lookup,find);
		printf("O(log n) %f %f %f -> %d\n",find[0],find[1],find[2],index);
    }

    mysofa_lookup_free(lookup);
    mysofa_free(hrtf);
	return 0;
}
