/*

  Copyright 2016 Christian Hoene, Symonics GmbH

*/

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "reader.h"

/* read superblock
   00000000  89 48 44 46 0d 0a 1a 0a  02 08 08 00 00 00 00 00  |.HDF............|
   00000010  00 00 00 00 ff ff ff ff  ff ff ff ff bc 62 12 00  |.............b..|
   00000020  00 00 00 00 30 00 00 00  00 00 00 00 27 33 a3 16  |....0.......'3..|
*/

int superblockRead(struct READER *reader, struct SUPERBLOCK *superblock) {
	char buf[8];
	memset(superblock, 0, sizeof(*superblock));

	/* signature */
	if (fread(buf, 1, 8, reader->fhd) != 8
	    || strncmp("\211HDF\r\n\032\n", buf, 8)) {
		log("file does not have correct signature");
		return MYSOFA_INVALID_FORMAT;
	}

	/* read version of superblock, must be 2 */
	if ((fgetc(reader->fhd) & ~1) != 2) {
		log("superblock must have version 2 or 3\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	superblock->size_of_offsets = (uint8_t)fgetc(reader->fhd);
	superblock->size_of_lengths = (uint8_t)fgetc(reader->fhd);
	if(fgetc(reader->fhd)<0) /* File Consistency Flags */
		return MYSOFA_READ_ERROR;

	if (superblock->size_of_offsets < 2 || superblock->size_of_offsets > 8
	    || superblock->size_of_lengths < 2
	    || superblock->size_of_lengths > 8) {
		log("size of offsets and length is invalid: %d %d\n",
		    superblock->size_of_offsets, superblock->size_of_lengths);
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	superblock->base_address = readValue(reader, superblock->size_of_offsets);
	superblock->superblock_extension_address = readValue(reader,
							     superblock->size_of_offsets);
	superblock->end_of_file_address = readValue(reader,
						    superblock->size_of_offsets);
	superblock->root_group_object_header_address = readValue(reader,
								 superblock->size_of_offsets);

	if (superblock->base_address != 0) {
		log("base address is not null\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	if (fseek(reader->fhd, 0L, SEEK_END))
		return errno;

	if (superblock->end_of_file_address != ftell(reader->fhd)) {
		log("file size mismatch\n");
		return MYSOFA_INVALID_FORMAT;

	}

	/* fseeko(fhd, 4, SEEK_CUR);  skip checksum */
	/* end of superblock */

	/* seek to first object */
	if (fseek(reader->fhd, superblock->root_group_object_header_address,
		  SEEK_SET)) {
		log("cannot seek to first object at %ld\n",
		    superblock->root_group_object_header_address);
		return errno;
	}

	return dataobjectRead(reader, &superblock->dataobject, NULL);
}

void superblockFree(struct READER *reader, struct SUPERBLOCK *superblock) {
	dataobjectFree(reader, &superblock->dataobject);
}
