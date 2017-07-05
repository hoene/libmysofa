// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <assert.h>
#include "..\..\src\hrtf\mysofa.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
	int err;

	struct MYSOFA_EASY *hrtf = (MYSOFA_EASY *)malloc(sizeof(struct MYSOFA_EASY));
	if (!hrtf) return 1234;

	hrtf->lookup = NULL;
	hrtf->neighborhood = NULL;

	hrtf->hrtf = mysofa_load("../../tests/FHK_HRIR_L2354.sofa", &err);
	if (!hrtf->hrtf) {
		mysofa_close(hrtf);
		return err;
	}

	err = mysofa_check(hrtf->hrtf);
	if (err != MYSOFA_OK) {
		mysofa_close(hrtf);
		return err;
	}

	mysofa_tocartesian(hrtf->hrtf);

	hrtf->lookup = mysofa_lookup_init(hrtf->hrtf);
	if (hrtf->lookup == NULL) {
		err = MYSOFA_INTERNAL_ERROR;
		mysofa_close(hrtf);
		return err;
	}
	hrtf->neighborhood = mysofa_neighborhood_init(hrtf->hrtf, hrtf->lookup);


	// general information about the loaded HRTF set
	fprintf(stderr, "Number of HRTFs: %d\n", hrtf->hrtf->M);
	fprintf(stderr, "Filter length: %d samples\n", hrtf->hrtf->N);
	fprintf(stderr, "Radius min: %f, max: %f\n", hrtf->lookup->radius_min, hrtf->lookup->radius_max);
	fprintf(stderr, "Database: %s\n", mysofa_getAttribute(hrtf->hrtf->attributes, "DatabaseName"));
	fprintf(stderr, "Listener: %s\n", mysofa_getAttribute(hrtf->hrtf->attributes, "ListenerShortName"));
	 
	// get index to a position given in t (in spherical)
	float t[3], a[3];
	t[0] = 0; // azimuth in deg
	t[1] = 0; // elevation in deg
	t[2] = 1.2f; // radius in m
	
	fprintf(stderr, "Wanted Position: %f, %f, %f\n", t[0], t[1], t[2]);
	mysofa_s2c(t);
	int nearest = mysofa_lookup(hrtf->lookup, t);
	fprintf(stderr, "Nearest position found at: %d\n", nearest);
	a[0] = hrtf->hrtf->SourcePosition.values[3 * nearest];
	a[1] = hrtf->hrtf->SourcePosition.values[3 * nearest+1];
	a[2] = hrtf->hrtf->SourcePosition.values[3 * nearest+2];
	mysofa_c2s(a);
	fprintf(stderr, "Actual Position: %f, %f, %f\n", a[0],a[1],a[2]);

	// get the HRIR for the position given in t
	float *IRL, *IRR, delL, delR;
	int size = hrtf->hrtf->N * hrtf->hrtf->R;
	IRL=hrtf->hrtf->DataIR.values + nearest*size;
	IRR= hrtf->hrtf->DataIR.values + nearest*size + hrtf->hrtf->N;

	// save the left HRIR in a file 
	ofstream myfile;
	myfile.open("hrtf.dat", ios::binary);
	myfile.write(reinterpret_cast<const char*>(IRL), hrtf->hrtf->N * sizeof(float));
	myfile.close();

	// close the file
	mysofa_close(hrtf);

	// wait for a key press
	while (!_kbhit());

    return 0;
}

