#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "tests.h"
#include "json.h"

void test_easy() {
	struct MYSOFA_EASY *easy;
	int err = 0;
	int filterlength;
	int filters=0;
	float theta,r;
	float *coordinates;
	float *ir;
	float *delays;
	int count=0;
	FILE *file;

	easy = mysofa_open("tests/sofa_api_mo_test/MIT_KEMAR_normal_pinna.sofa", 8000., &filterlength, &err);
	if (!easy) {
		CU_FAIL_FATAL("Error reading file.");
	}

	mysofa_close(easy);

	easy = mysofa_open("tests/sofa_api_mo_test/Pulse.sofa", 44100, &filterlength, &err);
	if (!easy) {
		CU_FAIL_FATAL("Error reading file.");
	}

	for(theta=-90.;theta<=90.;theta+=5.) {
		r = round(cos(theta*M_PI/180.) * 120.);
		if(r==0.) r=1;
		filters+=r;
	}
#ifdef VDEBUG
	printf("Filters %d\n",filters);
#endif

	coordinates = malloc(filters*sizeof(float)*3);
	ir = malloc(filters*easy->hrtf->N*sizeof(float)*2);
	delays = malloc(filters*2*sizeof(float));

	for(theta=-90.;theta<=90.;theta+=5.) {
		int r = round(cos(theta*M_PI/180.) * 120.);
		int phi;
		if(r==0) r=1;
		for(phi=0;phi<r;phi++) {
			coordinates[count*3+0] = phi * (360. / r);
			coordinates[count*3+1] = theta;
			coordinates[count*3+2] = 1;
			convertSphericalToCartesian(coordinates+count*3,3);
#ifdef VDEBUG
			printf("%f %d %d %f %f %f\n",theta,phi,count,coordinates[count*3+0],coordinates[count*3+1],coordinates[count*3+2]);
#endif
			mysofa_getfilter_float(easy,
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
	easy->hrtf->M = filters;

	file = fopen("easy.tmp.json","w");
	CU_ASSERT(file!=NULL);
	printJson(file,easy->hrtf);
	fclose(file);
	/* TODO verify correctness of the easy.json file */

	mysofa_close(easy);
}

