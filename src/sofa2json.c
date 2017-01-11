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

#include "mysofa.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void printString(char *string)
{
    putchar('"');
    if(string) {
        while(*string) {
        switch(*string) {
        case '"':
            printf("\\\"");
            break;
        case '\\':
            printf("\\\\");
            break;
        case '/':
            printf("\\/");
            break;
        case '\b':
            printf("\\b");
            break;
        case '\f':
            printf("\\f");
            break;
        case '\n':
            printf("\\n");
            break;
        case '\r':
            printf("\\r");
            break;
        case '\t':
            printf("\\t");
            break;
        default:
            putchar(*string);
        }
        string++;
        }
   }
   putchar('"');
}
    
static void printAttributes(int spaces, struct MYSOFA_ATTRIBUTE *attr)
{
        int i;
        
        for(i=0;i<spaces;i++) putchar(' ');
        printf("\"Attributes\": {\n");
        while(attr) {
            for(i=0;i<=spaces;i++) putchar(' ');
            printString(attr->name);
            printf(": ");
            printString(attr->value);
            printf("%s",attr->next?",\n":"\n");
            attr = attr->next;
        }
        for(i=0;i<spaces;i++) putchar(' ');
        printf("}");                
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
      
static void printDimensions(struct MYSOFA_HRTF *hrtf, struct MYSOFA_ATTRIBUTE **p)
{
    struct MYSOFA_ATTRIBUTE *found;
    char *s;
    int dimensions[4];
    int dims=0,i;
    
    while(*p) {
        if(!strcmp((*p)->name,"DIMENSION_LIST")) {
            found = *p;
            *p = (*p)->next;
            break;
            
        }
        p = &((*p)->next);
   }
   if(found) {
        
        printf("   \"DimensionNames\":[");
        s = found->value;
        while(s[0] && dims<4) {
            switch(s[0]) {
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
            if(s[1]==',') {
                printf("\"%c\",",s[0]);
                s+=2;
            }
            else {
                printf("\"%c\"",s[0]);
                break;
            }
        }
        printf("],\n");
        
        printf("   \"Dimensions\":[");
        for(i=0;i<dims;i++)  {
            if(i+1<dims)
                printf("%d,",dimensions[i]);
            else
                printf("%d],\n",dimensions[i]);
        }
           
        free(found->name);
        free(found->value);
        free(found);
   }
}
    
    
static int printArray(struct MYSOFA_HRTF *hrtf, struct MYSOFA_ARRAY *array, char *name)
{
    int i=0;

    if(!array->elements)
        return 0;
        
    printf("  ");
    printString(name);
    printf(": {\n");
    
    printf("   \"TypeName\":\"double\",\n");

    printDimensions(hrtf, &array->attributes);
    
    if(array->attributes) {
        printAttributes(3, array->attributes);
        printf(",\n");
    }
    
    printf("   \"Values\": [");
    for(i=0;i<array->elements;i++) {
            printf("%c%s%.6f",i==0?' ':',',i%20==19?"\n    ":"",array->values[i]);
    }
    
    printf(" ]\n  }");
    
    return 1;
}

int main(int argc, char **argv) {
	struct MYSOFA_HRTF *hrtf = NULL;
	int err = 0;    

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <FILE.SOFA>\n", argv[0]);
		return 1;
	}

	hrtf = mysofa_load(argv[1], &err);

	if (!hrtf) {
		fprintf(stderr, "Error reading file %s. Error code: %d\n", argv[1],
				err);
		return err;
	}

    err=mysofa_check(hrtf);
    printf("result %d\n",err);

#if 0

	/*
	 * The HRTF structure data types
	 */
	printf("{\n");
	
	if(hrtf->attributes) {
	    printAttributes(1,hrtf->attributes);
	    printf(",\n");
    }

    printf(" \"Dimensions\": {\n");
    printf("  \"I\": %d,\n",hrtf->I);	
    printf("  \"C\": %d,\n",hrtf->C);	
    printf("  \"R\": %d,\n",hrtf->R);
    printf("  \"E\": %d,\n",hrtf->E);	
    printf("  \"N\": %d,\n",hrtf->N);	
    printf("  \"M\": %d\n",hrtf->M);	
    printf(" },\n");

    printf(" \"Variables\": {\n");
    if(printArray(hrtf, &hrtf->ListenerPosition, "ListenerPosition"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->ReceiverPosition, "ReceiverPosition"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->SourcePosition, "SourcePosition"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->EmitterPosition, "EmitterPosition"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->ListenerUp, "ListenerUp"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->ListenerView, "ListenerView"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->DataIR, "Data.IR"))
        printf(",\n");
    if(printArray(hrtf, &hrtf->DataSamplingRate, "Data.SamplingRate"))
        printf(",\n");
    printArray(hrtf, &hrtf->DataDelay, "Data.Delay");
    printf(" }\n}\n");
#endif
	mysofa_free(hrtf);

	return 0;
}
