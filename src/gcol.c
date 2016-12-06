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

#include <stdlib.h>
#include <string.h>
#include "reader.h"


/*  III.E. Disk Format: Level 1E - Global Heap
 */

static int readGCOL(struct READER *reader) {

	uint16_t reference_count, address;
	uint64_t collection_size, end;
	struct GCOL *gcol;
    char buf[4];
    
    UNUSED(reference_count);

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "GCOL", 4)) {
		log("cannot read signature of global heap collection\n");
		return MYSOFA_INVALID_FORMAT;
	}

	if (fgetc(reader->fhd) != 1) {
		log("object GCOL must have version 1\n");
		return MYSOFA_INVALID_FORMAT;
	}
	fgetc(reader->fhd);
	fgetc(reader->fhd);
	fgetc(reader->fhd);

    address = ftello(reader->fhd);
	end = address;
	collection_size = readValue(reader, reader->superblock.size_of_lengths);
	end += collection_size - 8;

	while(ftello(reader->fhd) <= end - 8 - reader->superblock.size_of_lengths) {

		gcol = malloc(sizeof(*gcol));
		gcol->heap_object_index = readValue(reader, 2);
		if(gcol->heap_object_index == 0) {
		    free(gcol);
		    break;
		}
		reference_count = readValue(reader, 2);
		fseek(reader->fhd, 4, SEEK_CUR);
		gcol->object_size = readValue(reader, reader->superblock.size_of_lengths);
		gcol->value = readValue(reader, gcol->object_size);
		gcol->address = address;
		log(" GCOL object %d size %ld value %08lX\n", gcol->heap_object_index,
				gcol->object_size, gcol->value);
		

		gcol->next = reader->gcol;
		reader->gcol = gcol;
	}

	log(" END %08lX vs. %08lX\n", ftello(reader->fhd), end); /* bug in the normal hdf5 specification */
//	fseeko(reader->fhd, end, SEEK_SET);
	return MYSOFA_OK;
}

int gcolRead(struct READER *reader, uint64_t gcol, int reference, uint64_t *dataobject)
{
    uint64_t pos;
    struct GCOL *p = reader->gcol;
    
    while(p && p->address != gcol && p->heap_object_index != reference) {
        p = p->next;
    }
    if(!p) {
		pos = ftello(reader->fhd);
		fseeko(reader->fhd, gcol, SEEK_SET);
		readGCOL(reader);
		fseeko(reader->fhd, pos, SEEK_SET);

        p = reader->gcol;
        while(p && p->address != gcol && p->heap_object_index != reference) {
            p = p->next;
        }
        if(!p) {
            log("unknown gcol %lX %d\n",gcol,reference);
            return MYSOFA_INVALID_FORMAT;
        }
    }
    *dataobject = p->value;
    
    return MYSOFA_OK;
}
#if 0
        
		gcol = reader->gcol;
		for (;;) {
			if (gcol == NULL) {
				log("reference unknown!\n");
				return MYSOFA_INVALID_FORMAT;
			}
			if (gcol->heap_object_index == reference) {
			    log("found reference at %LX\n", gcol->object_pos);
			    break;
				pos = ftello(reader->fhd);
				fseeko(reader->fhd, gcol->object_pos, SEEK_SET);
				dt2 = *dt;
				dt2.list = 0;
				dt2.size = gcol->object_size;
				readDataVar(reader, &dt2, ds);
				fseeko(reader->fhd, pos, SEEK_SET);
				break;
			}
			gcol = gcol->next;
#endif


void gcolFree(struct GCOL *gcol) 
{
    if(gcol) {
        gcolFree(gcol->next);
        free(gcol);
   }
}

