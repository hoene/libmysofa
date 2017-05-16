/*

 Copyright 2016 Christian Hoene, Symonics GmbH

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

	address = ftell(reader->fhd);
	end = address;
	collection_size = readValue(reader, reader->superblock.size_of_lengths);
	end += collection_size - 8;

	while (ftell(reader->fhd) <= end - 8 - reader->superblock.size_of_lengths) {

		gcol = malloc(sizeof(*gcol));
		gcol->heap_object_index = readValue(reader, 2);
		if (gcol->heap_object_index == 0) {
			free(gcol);
			break;
		}
		reference_count = readValue(reader, 2);
		fseek(reader->fhd, 4, SEEK_CUR);
		gcol->object_size = readValue(reader,
				reader->superblock.size_of_lengths);
		gcol->value = readValue(reader, gcol->object_size);
		gcol->address = address;
		log(" GCOL object %d size %ld value %08lX\n", gcol->heap_object_index,
				gcol->object_size, gcol->value);

		gcol->next = reader->gcol;
		reader->gcol = gcol;
	}

	log(" END %08lX vs. %08lX\n", ftell(reader->fhd), end); /* bug in the normal hdf5 specification */
/*	fseek(reader->fhd, end, SEEK_SET); */
	return MYSOFA_OK;
}

int gcolRead(struct READER *reader, uint64_t gcol, int reference,
		uint64_t *dataobject) {
	uint64_t pos;
	struct GCOL *p = reader->gcol;

	while (p && p->address != gcol && p->heap_object_index != reference) {
		p = p->next;
	}
	if (!p) {
		pos = ftell(reader->fhd);
		fseek(reader->fhd, gcol, SEEK_SET);
		readGCOL(reader);
		fseek(reader->fhd, pos, SEEK_SET);

		p = reader->gcol;
		while (p && p->address != gcol && p->heap_object_index != reference) {
			p = p->next;
		}
		if (!p) {
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
		pos = ftell(reader->fhd);
		fseek(reader->fhd, gcol->object_pos, SEEK_SET);
		dt2 = *dt;
		dt2.list = 0;
		dt2.size = gcol->object_size;
		readDataVar(reader, &dt2, ds);
		fseek(reader->fhd, pos, SEEK_SET);
		break;
	}
	gcol = gcol->next;
#endif

void gcolFree(struct GCOL *gcol) {
	if (gcol) {
		gcolFree(gcol->next);
		free(gcol);
	}
}

