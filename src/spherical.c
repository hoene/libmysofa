#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mysofa.h"

static int changeAttribute(struct MYSOFA_ATTRIBUTE *attr, char *name, char *value, char *newvalue)
{
    while(attr) {
        if(!strcmp(name,attr->name) && (value==NULL || !strcmp(value,attr->value))) {
        	free(attr->value);
        	attr->value = strdup(newvalue);
            return 1;
        }
        attr = attr->next;
    }
    return 0;
}

static void convertCartesianToSpherical(double *values, int elements)
{
	int i;
	double x,y,z,r,theta, phi;

	for(i = 0; i < elements-2; i += 3) {
		x = values[i];
		y = values[i+1];
		z = values[i+2];
		r = sqrt(x*x + y*y + z*z);

		theta = atan2(z,sqrt(x*x + y*y));
		phi = atan2(y,x);

		values[i] = fmod(phi * 180 / M_PI + 360, 360);
		values[i+1] = theta * 180 / M_PI;
		values[i+2] = r;
	}
}

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
