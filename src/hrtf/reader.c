/*

 Copyright 2016 Christian Hoene, Symonics GmbH

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mysofa.h"
#include "../hdf/reader.h"

/* checks file address.
 * NULL is an invalid address indicating a invalid field
 */
int validAddress(struct READER *reader, uint64_t address) {
	return address > 0 && address < reader->superblock.end_of_file_address;
}

/* little endian */
uint64_t readValue(struct READER *reader, int size) {
	int i;
	uint64_t value = fgetc(reader->fhd);
	for (i = 1; i < size; i++)
		value |= ((uint64_t) fgetc(reader->fhd)) << (i * 8);
	return value;
}

static int mystrcmp(char *s1, char *s2) {
	if (s1 == NULL && s2 == NULL)
		return 0;
	if (s1 == NULL)
		return -1;
	if (s2 == NULL)
		return 1;
	return strcmp(s1, s2);
}

static int checkAttribute(struct MYSOFA_ATTRIBUTE *attribute, char *name,
		char *value) {
	while (attribute) {
		if (!mystrcmp(attribute->name, name)
				&& !mystrcmp(attribute->value, value))
			return MYSOFA_OK;
		attribute = attribute->next;
	}

	return MYSOFA_INVALID_FORMAT;
}

static int getDimension(unsigned *dim, struct DATAOBJECT *dataobject) {
	int err;
	struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;

	if (!!(err = checkAttribute(dataobject->attributes, "CLASS",
			"DIMENSION_SCALE")))
		return err;

	while (attr) {
		log(" %s=%s\n",attr->name,attr->value);

		if (!strcmp(attr->name, "NAME")
				&& !strncmp(attr->value,
						"This is a netCDF dimension but not a netCDF variable.",
						53)) {
			char *p = attr->value + strlen(attr->value) - 1;
			while (isdigit(*p)) {
				p--;
			}
			p++;
			*dim = atoi(p);
			log("NETCDF DIM %u\n",*dim);
			return MYSOFA_OK;
		}
		attr = attr->next;
	}
	return MYSOFA_INVALID_FORMAT;
}

static int getArray(struct MYSOFA_ARRAY *array, struct DATAOBJECT *dataobject) {
	struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;
	while (attr) {
		log(" %s=%s\n",attr->name,attr->value);

		attr = attr->next;
	}

	if (dataobject->dt.f.bit_precision != 64)
		return MYSOFA_UNSUPPORTED_FORMAT;

	array->attributes = dataobject->attributes;
	dataobject->attributes = NULL;
	array->values = dataobject->data;
	dataobject->data = NULL;
	array->elements = dataobject->data_len / 8;
	return MYSOFA_OK;
}

static struct MYSOFA_HRTF *getHrtf(struct READER *reader, int *err) {
	int dimensionflags = 0;
	struct DIR *dir = reader->superblock.dataobject.directory;

	struct MYSOFA_HRTF *hrtf = malloc(sizeof(struct MYSOFA_HRTF));
	if (!hrtf) {
		*err = errno;
		return NULL;
	}
	memset(hrtf, 0, sizeof(struct MYSOFA_HRTF));

	/* copy SOFA file attributes */
	hrtf->attributes = reader->superblock.dataobject.attributes;
	reader->superblock.dataobject.attributes = NULL;

	/* check SOFA file attributes */
	if (!!(*err = checkAttribute(hrtf->attributes, "Conventions", "SOFA")))
		goto error;

	/* read dimensions */
	while (dir) {
		if (dir->dataobject.name && dir->dataobject.name[0]
				&& dir->dataobject.name[1] == 0) {
			switch (dir->dataobject.name[0]) {
			case 'I':
				*err = getDimension(&hrtf->I, &dir->dataobject);
				dimensionflags |= 1;
				break;
			case 'C':
				*err = getDimension(&hrtf->C, &dir->dataobject);
				dimensionflags |= 2;
				break;
			case 'R':
				*err = getDimension(&hrtf->R, &dir->dataobject);
				dimensionflags |= 4;
				break;
			case 'E':
				*err = getDimension(&hrtf->E, &dir->dataobject);
				dimensionflags |= 8;
				break;
			case 'N':
				*err = getDimension(&hrtf->N, &dir->dataobject);
				dimensionflags |= 0x10;
				break;
			case 'M':
				*err = getDimension(&hrtf->M, &dir->dataobject);
				dimensionflags |= 0x20;
				break;
			case 'S':
				break;  // be graceful, some issues with API version 0.4.4
			default:
				log("UNKNOWN SOFA VARIABLE %s", dir->dataobject.name);
				goto error;
			}
			if (*err)
				goto error;
		}
		dir = dir->next;
	}

	if (dimensionflags != 0x3f || hrtf->I != 1 || hrtf->C != 3) {
		log("dimensions are missing or wrong\n");
		goto error;
	}

	dir = reader->superblock.dataobject.directory;
	while (dir) {

		if (!strcmp(dir->dataobject.name, "ListenerPosition")) {
			*err = getArray(&hrtf->ListenerPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ReceiverPosition")) {
			*err = getArray(&hrtf->ReceiverPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "SourcePosition")) {
			*err = getArray(&hrtf->SourcePosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "EmitterPosition")) {
			*err = getArray(&hrtf->EmitterPosition, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ListenerUp")) {
			*err = getArray(&hrtf->ListenerUp, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "ListenerView")) {
			*err = getArray(&hrtf->ListenerView, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.IR")) {
			*err = getArray(&hrtf->DataIR, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.SamplingRate")) {
			*err = getArray(&hrtf->DataSamplingRate, &dir->dataobject);
		} else if (!strcmp(dir->dataobject.name, "Data.Delay")) {
			*err = getArray(&hrtf->DataDelay, &dir->dataobject);
		} else {
			if (!(dir->dataobject.name[0] && !dir->dataobject.name[1]))
				log("UNKNOWN SOFA VARIABLE %s.\n", dir->dataobject.name);
		}
		dir = dir->next;
	}

	return hrtf;

	error: free(hrtf);
	if (!*err)
		*err = MYSOFA_INVALID_FORMAT;
	return NULL;
}

struct MYSOFA_HRTF* mysofa_load(char *filename, int *err) {
	struct READER reader;
	struct MYSOFA_HRTF *hrtf = NULL;

	reader.fhd = fopen(filename, "rb");

	if (!reader.fhd) {
		log("cannot open file %s\n", filename);
		*err = errno;
		return NULL;
	}
	reader.gcol = NULL;
	reader.all = NULL;

	*err = superblockRead(&reader, &reader.superblock);

	if (!*err) {
		hrtf = getHrtf(&reader, err);
	}

	superblockFree(&reader, &reader.superblock);
	gcolFree(reader.gcol);
	fclose(reader.fhd);

	return hrtf;
}

static void arrayFree(struct MYSOFA_ARRAY *array) {
	while (array->attributes) {
		struct MYSOFA_ATTRIBUTE *next = array->attributes->next;
		free(array->attributes->name);
		free(array->attributes->value);
		free(array->attributes);
		array->attributes = next;
	}
	free(array->values);
}

void mysofa_free(struct MYSOFA_HRTF *hrtf) {
	if (!hrtf)
		return;

	while (hrtf->attributes) {
		struct MYSOFA_ATTRIBUTE *next = hrtf->attributes->next;
		free(hrtf->attributes->name);
		free(hrtf->attributes->value);
		free(hrtf->attributes);
		hrtf->attributes = next;
	}

	arrayFree(&hrtf->ListenerPosition);
	arrayFree(&hrtf->ReceiverPosition);
	arrayFree(&hrtf->SourcePosition);
	arrayFree(&hrtf->EmitterPosition);
	arrayFree(&hrtf->ListenerUp);
	arrayFree(&hrtf->ListenerView);
	arrayFree(&hrtf->DataIR);
	arrayFree(&hrtf->DataSamplingRate);
	arrayFree(&hrtf->DataDelay);
	free(hrtf);
}
