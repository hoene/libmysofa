#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdlib.h>
#include "mysofa.h"
#include "tools.h"

int main() {
	struct MYSOFA_EASY *easy;
	int err = 0;
	int filterlength;

	easy = mysofa_open("../../tests/sofa_api_mo_test/Pulse.sofa", 44100, &filterlength, &err);
	if (!easy)
		easy = mysofa_open("tests/sofa_api_mo_test/Pulse.sofa", 44100, &filterlength, &err);
	if (!easy) {
		fprintf(stderr, "Error reading file. Error code: %d\n", err);
		return err;
	}

	int filters=0;
	double theta,r;
	for(theta=-90.;theta<=90.;theta+=5.) {
		r = round(cos(theta*M_PI/180.) * 120.);
		if(r==0.) r=1;
		filters+=r;
	}
	printf("Filters %d\n",filters);

	double *coordinates = malloc(filters*sizeof(double)*3);
	double *ir = malloc(filters*easy->hrtf->N*sizeof(double)*2);
	double *delays = malloc(filters*2*sizeof(double));

	int count=0;
	for(theta=-90.;theta<=90.;theta+=5.) {
		int r = round(cos(theta*M_PI/180.) * 120.);
		if(r==0) r=1;
		int phi;
		for(phi=0;phi<r;phi++) {
			coordinates[count*3+0] = phi * (360. / r);
			coordinates[count*3+1] = theta;
			coordinates[count*3+2] = 1;
			convertSphericalToCartesian(coordinates+count*3,3);
			printf("%f %d %d %f %f %f\n",theta,phi,count,coordinates[count*3+0],coordinates[count*3+1],coordinates[count*3+2]);
			mysofa_getfilter_double(easy,
					coordinates[count*3+0],
					coordinates[count*3+1],
					coordinates[count*3+2],
					ir + 2*count * easy->hrtf->N,
					ir + (2*count+1) * easy->hrtf->N,
					&delays[2*count], &delays[2*count+1]);
			count++;
		}
	}

	free(easy->hrtf->DataDelay.values);
	free(easy->hrtf->DataIR.values);
	free(easy->hrtf->SourcePosition.values);
	easy->hrtf->DataDelay.elements=filters*2;
	easy->hrtf->DataDelay.values=delays;
	easy->hrtf->DataIR.elements=filters*2*easy->hrtf->N;
	easy->hrtf->DataIR.values=ir;
	easy->hrtf->SourcePosition.elements=filters*3;
	easy->hrtf->SourcePosition.values=coordinates;

	json(easy->hrtf);

	mysofa_close(easy);

	return 0;
}
