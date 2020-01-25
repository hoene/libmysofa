#include "mysofa.h"
#include "mysofa_export.h"
#include "tools.h"
#include <math.h>
#include <string.h>

static int compareValues(struct MYSOFA_ARRAY *array, const float *compare,
                         int elements) {
  int i;
  if (array->values == NULL || array->elements != elements)
    return 0;
  for (i = 0; i < elements; i++)
    if (!fequals(array->values[i], compare[i]))
      return 0;
  return 1;
}

static const float array000[] = {0, 0, 0};
static const float array001[] = {0, 0, 1};
static const float array100[] = {1, 0, 0};

MYSOFA_EXPORT int mysofa_check(struct MYSOFA_HRTF *hrtf) {

  /* check for valid parameter ranges */
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
  if (!verifyAttribute(hrtf->attributes, "Conventions", "SOFA") ||
      !verifyAttribute(hrtf->attributes, "SOFAConventions",
                       "SimpleFreeFieldHRIR") ||

      /* TODO: Support FT too */
      !verifyAttribute(hrtf->attributes, "DataType", "FIR") ||
      !verifyAttribute(hrtf->attributes, "RoomType", "free field"))

    return MYSOFA_INVALID_ATTRIBUTES;

  /*==============================================================================
   dimensions
   ==============================================================================
 */

  if (hrtf->C != 3 || hrtf->I != 1 || hrtf->E != 1 || hrtf->R != 2)
    return MYSOFA_INVALID_DIMENSIONS;

  /* verify format */

  if (hrtf->ListenerView.values) {
    if (!verifyAttribute(hrtf->ListenerView.attributes, "DIMENSION_LIST",
                         "I,C"))
      return MYSOFA_INVALID_DIMENSION_LIST;
    if (verifyAttribute(hrtf->ListenerView.attributes, "Type", "cartesian")) {
      if (!compareValues(&hrtf->ListenerView, array100, 3))
        return MYSOFA_INVALID_FORMAT;
    } else if (verifyAttribute(hrtf->ListenerView.attributes, "Type",
                               "spherical")) {
      if (!compareValues(&hrtf->ListenerView, array001, 3))
        return MYSOFA_INVALID_FORMAT;
    } else
      return MYSOFA_INVALID_COORDINATE_TYPE;
  }

#if 0
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

	/* TODO. support M,C too */
	if(!verifyAttribute(hrtf->ListenerPosition.attributes,"DIMENSION_LIST","I,C"))
	return MYSOFA_INVALID_FORMAT;
	if(!compareValues(&hrtf->ListenerPosition,array000,3))
	return MYSOFA_INVALID_FORMAT;
#endif

  /* TODO: support ECM too */
  if (!verifyAttribute(hrtf->EmitterPosition.attributes, "DIMENSION_LIST",
                       "E,C,I"))
    return MYSOFA_ONLY_EMITTER_WITH_ECI_SUPPORTED;
  if (!compareValues(&hrtf->EmitterPosition, array000, 3))
    return MYSOFA_ONLY_EMITTER_WITH_ECI_SUPPORTED;

  if (hrtf->DataDelay.values) {
    if (!verifyAttribute(hrtf->DataDelay.attributes, "DIMENSION_LIST", "I,R") &&
        !verifyAttribute(hrtf->DataDelay.attributes, "DIMENSION_LIST", "M,R"))
      return MYSOFA_ONLY_DELAYS_WITH_IR_OR_MR_SUPPORTED;
  }

  /* TODO: Support different sampling rate per measurement, support default
   sampling rate of 48000 However, so far, I have not seen any sofa files with
   an format other and I */
  if (!verifyAttribute(hrtf->DataSamplingRate.attributes, "DIMENSION_LIST",
                       "I"))
    return MYSOFA_ONLY_THE_SAME_SAMPLING_RATE_SUPPORTED;

  if (!verifyAttribute(hrtf->ReceiverPosition.attributes, "DIMENSION_LIST",
                       "R,C,I"))
    return MYSOFA_RECEIVERS_WITH_RCI_SUPPORTED;
  if (!verifyAttribute(hrtf->ReceiverPosition.attributes, "Type", "cartesian"))
    return MYSOFA_RECEIVERS_WITH_CARTESIAN_SUPPORTED;

  if (!fequals(hrtf->ReceiverPosition.values[0], 0.) ||
      hrtf->ReceiverPosition.values[1] > 0 ||
      !fequals(hrtf->ReceiverPosition.values[2], 0.) ||
      !fequals(hrtf->ReceiverPosition.values[3], 0.) ||
      !fequals(hrtf->ReceiverPosition.values[4],
               -hrtf->ReceiverPosition.values[1]) ||
      !fequals(hrtf->ReceiverPosition.values[5], 0.)) {
    return MYSOFA_INVALID_RECEIVER_POSITIONS;
  }

  /* read source positions */
  if (!verifyAttribute(hrtf->SourcePosition.attributes, "DIMENSION_LIST",
                       "M,C"))
    return MYSOFA_ONLY_SOURCES_WITH_MC_SUPPORTED;

  return MYSOFA_OK;
}
