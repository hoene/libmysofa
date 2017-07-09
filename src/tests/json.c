/*
 * json.c
 *
 *  Created on: 20.01.2017
 *      Author: hoene
 */

#include <stdio.h>
#include <string.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "json.h"

static void printString(FILE *out, char *string) {
	fprintf(out, "\"");
	if (string) {
		while (*string) {
			switch (*string) {
			case '"':
				fprintf(out, "\\\"");
				break;
			case '\\':
				fprintf(out, "\\\\");
				break;
			case '/':
				fprintf(out, "\\/");
				break;
			case '\b':
				fprintf(out, "\\b");
				break;
			case '\f':
				fprintf(out, "\\f");
				break;
			case '\n':
				fprintf(out, "\\n");
				break;
			case '\r':
				fprintf(out, "\\r");
				break;
			case '\t':
				fprintf(out, "\\t");
				break;
			default:
				fprintf(out, "%c", *string);
			}
			string++;
		}
	}
	fprintf(out, "\"");
}

static void printAttributes(FILE *out, int spaces,
			    struct MYSOFA_ATTRIBUTE *attr) {
	int i;

	for (i = 0; i < spaces; i++)
		fprintf(out, " ");
	fprintf(out, "\"Attributes\": {\n");
	while (attr) {
		for (i = 0; i <= spaces; i++)
			fprintf(out, " ");
		printString(out, attr->name);
		fprintf(out, ": ");
		printString(out, attr->value);
		fprintf(out, "%s", attr->next ? ",\n" : "\n");
		attr = attr->next;
	}
	for (i = 0; i < spaces; i++)
		fprintf(out, " ");
	fprintf(out, "}");
}

/*
  "Dimensions":[
  1,
  2
  ],
  "DimensionNames":[
  "I",
  "R"
  ],
*/

static void printDimensions(FILE *out, struct MYSOFA_HRTF *hrtf,
			    struct MYSOFA_ATTRIBUTE **p) {
	struct MYSOFA_ATTRIBUTE *found = NULL;
	char *s;
	int dimensions[4];
	int dims = 0, i;

	while (*p) {
		if (!strcmp((*p)->name, "DIMENSION_LIST")) {
			found = *p;
			*p = (*p)->next;
			break;

		}
		p = &((*p)->next);
	}
	if (found) {

		fprintf(out, "   \"DimensionNames\":[");
		s = found->value;
		while (s[0] && dims < 4) {
			switch (s[0]) {
			case 'I':
				dimensions[dims++] = hrtf->I;
				break;
			case 'C':
				dimensions[dims++] = hrtf->C;
				break;
			case 'R':
				dimensions[dims++] = hrtf->R;
				break;
			case 'E':
				dimensions[dims++] = hrtf->E;
				break;
			case 'N':
				dimensions[dims++] = hrtf->N;
				break;
			case 'M':
				dimensions[dims++] = hrtf->M;
				break;
			}
			if (s[1] == ',') {
				fprintf(out, "\"%c\",", s[0]);
				s += 2;
			} else {
				fprintf(out, "\"%c\"", s[0]);
				break;
			}
		}
		fprintf(out, "],\n");

		fprintf(out, "   \"Dimensions\":[");
		for (i = 0; i < dims; i++) {
			if (i + 1 < dims)
				fprintf(out, "%d,", dimensions[i]);
			else
				fprintf(out, "%d],\n", dimensions[i]);
		}

		free(found->name);
		free(found->value);
		free(found);
	}
}

static int printArray(FILE *out, struct MYSOFA_HRTF *hrtf,
		      struct MYSOFA_ARRAY *array, char *name) {
	int i = 0;

	if (!array->elements)
		return 0;

	fprintf(out, "  ");
	printString(out, name);
	fprintf(out, ": {\n");

	fprintf(out, "   \"TypeName\":\"double\",\n");

	printDimensions(out, hrtf, &array->attributes);

	if (array->attributes) {
		printAttributes(out, 3, array->attributes);
		fprintf(out, ",\n");
	}

	fprintf(out, "   \"Values\": [");
	for (i = 0; i < array->elements; i++) {
		fprintf(out, "%c%s%.6f", i == 0 ? ' ' : ',',
			i % 20 == 19 ? "\n    " : "", array->values[i]);
	}

	fprintf(out, " ]\n  }");

	return 1;
}

/*
 * The HRTF structure data types
 */
void printJson(FILE *out, struct MYSOFA_HRTF *hrtf) {
	fprintf(out, "{\n");

	if (hrtf->attributes) {
		printAttributes(out, 1, hrtf->attributes);
		fprintf(out, ",\n");
	}

	fprintf(out, " \"Dimensions\": {\n");
	fprintf(out, "  \"I\": %d,\n", hrtf->I);
	fprintf(out, "  \"C\": %d,\n", hrtf->C);
	fprintf(out, "  \"R\": %d,\n", hrtf->R);
	fprintf(out, "  \"E\": %d,\n", hrtf->E);
	fprintf(out, "  \"N\": %d,\n", hrtf->N);
	fprintf(out, "  \"M\": %d\n", hrtf->M);
	fprintf(out, " },\n");

	fprintf(out, " \"Variables\": {\n");
	if (printArray(out, hrtf, &hrtf->ListenerPosition, "ListenerPosition"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->ReceiverPosition, "ReceiverPosition"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->SourcePosition, "SourcePosition"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->EmitterPosition, "EmitterPosition"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->ListenerUp, "ListenerUp"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->ListenerView, "ListenerView"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->DataIR, "Data.IR"))
		fprintf(out, ",\n");
	if (printArray(out, hrtf, &hrtf->DataSamplingRate, "Data.SamplingRate"))
		fprintf(out, ",\n");
	printArray(out, hrtf, &hrtf->DataDelay, "Data.Delay");
	fprintf(out, " }\n}\n");
}
