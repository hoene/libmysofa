/*
 Copyright (c) 2016, Christian Hoene, Symonics GmbH
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 1. Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 3. All advertising materials mentioning features or use of this software
 must display the following acknowledgement:
 This product includes software developed by the Symonics GmbH.
 4. Neither the name of the Symonics GmbH nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
    
	mysofa_free(hrtf);

	return 0;
}
