#include <string.h>
#include "mysofa.h"

static int verifyAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value)
{
    while(attr) {
        if(!strcmp(name,attr->name) && !strcmp(value,attr->value))
            return 1;
        attr = attr->next;
    }
    return 0;
}

static int compareValues(struct MYSOFA_ARRAY *array, double *compare, int elements)
{
	int i;
	if(array->values==NULL || array->elements != elements)
		return 0;
	for(i=0;i<elements;i++)
		if(array->values[i]!=compare[i])
			return 0;
	return 1;
}

static double array000[] = { 0, 0, 0 };
static double array001[] = { 0, 0, 1 };
static double array0901[] = { 0, 90, 1 };
static double array100[] = { 1, 0, 0 };

int mysofa_check(struct MYSOFA_HRTF *hrtf)
{

    // check for valid parameter ranges
/*
	Attributes":{
	    "APIName":"ARI SOFA API for Matlab\/Octave",
	    "APIVersion":"0.4.0",
	    "ApplicationName":"Demo of the SOFA API",
	    "ApplicationVersion":"0.4.0",
	    "AuthorContact":"piotr@majdak.com",
	    "Comment":"",
	    "Conventions":"SOFA",
	    "DataType":"FIR",
	    "DatabaseName":"ARI",
	    "DateCreated":"2014-03-20 17:35:22",
	    "DateModified":"2014-03-20 17:35:22",
	    "History":"Converted from the ARI format",
	    "License":"No license provided, ask the author for permission",
	    "ListenerShortName":"",
	    "Organization":"Acoustics Research Institute",
	    "Origin":"",
	    "References":"",
	    "RoomType":"free field",
	    "SOFAConventions":"SimpleFreeFieldHRIR",
	    "SOFAConventionsVersion":"0.4",
	    "Title":"",
	    "Version":"0.6"
	  },
*/
	if(!verifyAttribute(hrtf->attributes,"Conventions","SOFA") ||
		!verifyAttribute(hrtf->attributes,"SOFAConventions","SimpleFreeFieldHRIR") ||

		// TODO: Support FT too
		!verifyAttribute(hrtf->attributes,"DataType","FIR") ||
		!verifyAttribute(hrtf->attributes,"RoomType","free field"))

			return MYSOFA_INVALID_FORMAT;

    //==============================================================================
    // dimensions
    //==============================================================================

	if(hrtf->C != 3 || hrtf->I != 1 || hrtf->E != 1 || hrtf->R != 2)
		return MYSOFA_INVALID_FORMAT;

//	size_t numMeasurements = file.GetDimension("M");
//	size_t numDataSamples = file.GetDimension("N");

	/* verify format */

	if(hrtf->ListenerView.values) {
		if(!verifyAttribute(hrtf->ListenerView.attributes,"DIMENSION_LIST","I,C"))
			return MYSOFA_INVALID_FORMAT;
		if(verifyAttribute(hrtf->ListenerView.attributes,"Type","cartesian")) {
			if(!compareValues(&hrtf->ListenerView,array100,3))
				return MYSOFA_INVALID_FORMAT;
		}
		else if(verifyAttribute(hrtf->ListenerView.attributes,"Type","spherical")) {
			if(!compareValues(&hrtf->ListenerView,array001,3))
				return MYSOFA_INVALID_FORMAT;
		}
		else
			return MYSOFA_INVALID_FORMAT;
	}

	if(hrtf->ListenerUp.values) {
		if(!verifyAttribute(hrtf->ListenerUp.attributes,"DIMENSION_LIST","I,C"))
			return MYSOFA_INVALID_FORMAT;
		if(verifyAttribute(hrtf->ListenerUp.attributes,"Type","cartesian")) {
			if(!compareValues(&hrtf->ListenerUp,array001,3))
				return MYSOFA_INVALID_FORMAT;
		}
		else if(verifyAttribute(hrtf->ListenerUp.attributes,"Type","spherical")) {
			if(!compareValues(&hrtf->ListenerUp,array0901,3))
				return MYSOFA_INVALID_FORMAT;
		}
	}

	// TODO. support M,C too
		if(!verifyAttribute(hrtf->ListenerPosition.attributes,"DIMENSION_LIST","I,C"))
			return MYSOFA_INVALID_FORMAT;
		if(!compareValues(&hrtf->ListenerPosition,array000,3))
			return MYSOFA_INVALID_FORMAT;

		// TODO: support ECM too
		if(!verifyAttribute(hrtf->EmitterPosition.attributes,"DIMENSION_LIST","E,C,I"))
			return MYSOFA_INVALID_FORMAT;
		if(!compareValues(&hrtf->EmitterPosition,array000,3))
			return MYSOFA_INVALID_FORMAT;

	// TODO: Support data delays for each filter
	// However, so far, I have not seen any sofa files with an format other and I,R
	if(hrtf->DataDelay.values) {
		if(!verifyAttribute(hrtf->DataDelay.attributes,"DIMENSION_LIST","I,R"))
			return MYSOFA_INVALID_FORMAT;
	}

	// TODO: Support different sampling rate per measurement, support default sampling rate of 48000
	// However, so far, I have not seen any sofa files with an format other and I
	if(!verifyAttribute(hrtf->DataSamplingRate.attributes,"DIMENSION_LIST","I"))
		return MYSOFA_INVALID_FORMAT;

	if(!verifyAttribute(hrtf->ReceiverPosition.attributes,"DIMENSION_LIST","R,C,I"))
		return MYSOFA_INVALID_FORMAT;
	if(!verifyAttribute(hrtf->ListenerUp.attributes,"Type","cartesian"))
		return MYSOFA_INVALID_FORMAT;

	if()
		!fequals(values[0],0) || values[1] > 0 || !fequals(values[2],0) ||
		!fequals(values[3],0) || !fequals(values[4],-values[1]) || !fequals(values[5],0)) {
		fprintf(stderr, "Error: Expecting proper ReceiverPosition: is %f %f %f %f %f %f\n",values[0],values[1],values[2],values[3],-values[4],values[5]);
            	return 0;
	}

#if 0

	/* read hrtf */
	std::vector< double > values;
	HrirDataT hData;

	hData.mIrPoints = numDataSamples;
	hData.mIrSize = 0;
	hData.mIrCount = 0;
	hData.mEvCount = 0;
	hData.mRadius = 0;
	hData.mDistance = 0;	

	hData.mFftSize = 1;
	while(hData.mFftSize < numDataSamples*4)
		hData.mFftSize *= 2;
        hData.mIrSize = 1 + (hData.mFftSize / 2);

	if(fftSize>hData.mFftSize) {
                hData.mFftSize = fftSize;
                hData.mIrSize = 1 + (fftSize / 2);
	}

	

	hData.mIrRate = values[0];

	file.GetValues(values, "ReceiverPosition");
	if(!verifyVariable(file,"ReceiverPosition","double","R,C,I",6,NULL,ATTR_TYPE_CARTESIAN) ||
		!fequals(values[0],0) || values[1] > 0 || !fequals(values[2],0) ||
		!fequals(values[3],0) || !fequals(values[4],-values[1]) || !fequals(values[5],0)) {
		fprintf(stderr, "Error: Expecting proper ReceiverPosition: is %f %f %f %f %f %f\n",values[0],values[1],values[2],values[3],-values[4],values[5]);
            	return 0;
	}
	hData.mRadius = values[4];

	// read source positions
	file.GetValues(values, "SourcePosition");
	if(verifyVariable(file,"SourcePosition","double","M,C", numMeasurements*3, NULL,  ATTR_TYPE_CARTESIAN)) {
		convertCartesianToSpherical(values);
	}
	else if(!verifyVariable(file,"SourcePosition","double","M,C", numMeasurements*3, NULL, ATTR_TYPE_SPHERICAL)) {
		fprintf(stderr, "Error: Expecting SourcePosition\n");
            	return 0;
	}

	convertSOFAtoOpenAL(values);

	hData.mDistance = getMeanDistance(values);
	output << "Mean Distance " << hData.mDistance << std::endl;
	sortSourcePositions(values);
	countElevationsAndAzimuths(values, hData);
        SOFA_ASSERT( hData.mIrCount == numMeasurements  );

	// read FIR filters
	std::vector< double > data;
	file.GetValues(data, "Data.IR");
	if(!verifyVariable(file,"Data.IR","double","M,R,N", numMeasurements*2*numDataSamples)) {
		fprintf(stderr, "Error: Expecting proper Data.IR\n");
            	return 0;

	}

	hData.mStereo = 1;
	hData.mHrirs = CreateArray(hData.mIrCount * hData.mIrSize * 2);
	readData(values,data,hData);

	// verify OpenAL parameters
	if(hData.mIrRate < MIN_RATE || hData.mIrRate > MAX_RATE) {
		fprintf(stderr, "Error: Sampling rate is not within proper limits: %u vs %d to %d\n",hData.mIrRate, MIN_RATE, MAX_RATE);
            	return 0;
	}

	if(hData.mIrPoints < MIN_POINTS || hData.mIrPoints > MAX_POINTS) {
		fprintf(stderr, "Error: FIR filter length is not within proper limits: %u vs %d to %d\n",hData.mIrPoints, MIN_POINTS, MAX_POINTS);
            	return 0;
	}

	if(hData.mEvCount < MIN_EV_COUNT || hData.mEvCount > MAX_EV_COUNT) {
		fprintf(stderr, "Error: Number of elevations is not within proper limits: %u vs %d to %d\n",hData.mEvCount, MIN_EV_COUNT, MAX_EV_COUNT);
            	return 0;
	}

	for(size_t i = 0; i < hData.mEvCount; i++) {
		if(hData.mAzCount[i] < MIN_AZ_COUNT || hData.mAzCount[i] > MAX_AZ_COUNT) {
			fprintf(stderr, "Error: Number of azimuths is not within proper limits: %u at %lu vs %d to %d\n", hData.mAzCount[i], i, MIN_AZ_COUNT, MAX_AZ_COUNT);
	            	return 0;
		}
	}

	if(hData.mRadius < MIN_RADIUS || hData.mRadius > MAX_RADIUS) {
		fprintf(stderr, "Error: Radius is not within proper limits: %f vs %f to %f\n",hData.mRadius, MIN_RADIUS, MAX_RADIUS);
            	return 0;
	}

	if(hData.mDistance < MIN_DISTANCE || hData.mDistance > MAX_DISTANCE) {
		fprintf(stderr, "Error: Distance is not within proper limits: %f vs %f to %f\n",hData.mDistance, MIN_DISTANCE, MAX_DISTANCE);
            	return 0;
	}


	// TODO consider read time delays from sofa file
    	hData.mHrtds = CreateArray(hData.mIrCount * 2);
	for(size_t ei=0;ei<hData.mEvCount;ei++) {
		for(size_t ai=0;ai<hData.mAzCount[ei];ai++) {
			AverageHrirOnset(hData.mHrirs + (hData.mEvOffset[ei] + ai) * hData.mIrSize, 1, ei, ai, &hData);
			AverageHrirOnset(hData.mHrirs + (hData.mEvOffset[ei] + ai + hData.mIrCount) * hData.mIrSize, 1, ei, ai, &hData);
			AverageHrirMagnitude(hData.mHrirs + (hData.mEvOffset[ei] + ai) * hData.mIrSize, 1, ei, ai, &hData);
// TODO: is the following correct for stereo?
			AverageHrirMagnitude(hData.mHrirs + (hData.mEvOffset[ei] + ai + hData.mIrCount) * hData.mIrSize, 1, ei, ai, &hData);
		}
	}

    	return hrtfPostProcessing(outRate, equalize, surface, limit, truncSize, HM_DATASET, radius, outFormat, outName, &hData);
}


#endif
    


    // if needed, sampling rate convertion

    // if needed, minphase filter

    // amplitute normalization (sum of frontal hearing is 1)

    // to frequency domain

    // remove phase

    // if wanted, to time domain

    // coordinates convertion to xyz

    // sorting coordinates for better searching

    // adding neighbors for interpolation

	return MYSOFA_OK;
}
