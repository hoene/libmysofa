#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mysofa.h"
#include "tools.h"

static void convertArray(struct MYSOFA_ARRAY *array)
{
	if(!changeAttribute(array->attributes,"Type","cartesian","spherical"))
		return;

	changeAttribute(array->attributes,"Units",NULL,"degree, degree, meter");

	convertCartesianToSpherical(array->values, array->elements);
}

void mysofa_tospherical(struct MYSOFA_HRTF *hrtf)
{
	convertArray(&hrtf->ListenerView);
	convertArray(&hrtf->ListenerUp);
	convertArray(&hrtf->ListenerPosition);
	convertArray(&hrtf->EmitterPosition);
	convertArray(&hrtf->ReceiverPosition);
	convertArray(&hrtf->SourcePosition);
}
