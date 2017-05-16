/*

 Copyright 2016 Christian Hoene, Symonics GmbH
 
*/

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "reader.h"

/*
 *
 00000370  42 54 4c 46 00 08 00 5b  01 00 00 00 2d 00 00 07  |BTLF...[....-...|
 00000380  00 00 00 f8 ea 72 15 00  c9 03 00 00 00 26 00 00  |.....r.......&..|
 00000390  14 00 00 00 32 32 7c 17  00 22 02 00 00 00 32 00  |....22|.."....2.|
 000003a0  00 0b 00 00 00 07 ef 9c  26 00 bb 01 00 00 00 46  |........&......F|
 000003b0  00 00 09 00 00 00 e5 f6  ba 26 00 45 03 00 00 00  |.........&.E....|
 000003c0  34 00 00 11 00 00 00 f6  71 f0 2e 00 a3 02 00 00  |4.......q.......|
 000003d0  00 3e 00 00 0d 00 00 00  61 36 dc 36 00 79 03 00  |.>......a6.6.y..|
 000003e0  00 00 35 00 00 12 00 00  00 97 1b 4e 45 00 88 01  |..5........NE...|
 000003f0  00 00 00 33 00 00 08 00  00 00 56 d7 d0 47 00 ae  |...3......V..G..|
 00000400  03 00 00 00 1b 00 00 13  00 00 00 2f 03 50 5a 00  |.........../.PZ.|
 00000410  22 01 00 00 00 39 00 00  06 00 00 00 b7 88 37 66  |"....9........7f|
 00000420  00 01 03 00 00 00 28 00  00 0f 00 00 00 dc aa 47  |......(........G|
 00000430  66 00 16 04 00 00 00 2c  00 00 15 00 00 00 6b 54  |f......,......kT|
 00000440  7d 77 00 fd 00 00 00 00  25 00 00 05 00 00 00 7d  |}w......%......}|
 00000450  0c 8c 9e 00 29 03 00 00  00 1c 00 00 10 00 00 00  |....)...........|
 00000460  4c f3 0e a0 00 16 00 00  00 00 25 00 00 00 00 00  |L.........%.....|
 00000470  00 e7 30 2d ab 00 01 02  00 00 00 21 00 00 0a 00  |..0-.......!....|
 00000480  00 00 35 b5 69 b0 00 e1  02 00 00 00 20 00 00 0e  |..5.i....... ...|
 00000490  00 00 00 2b c5 8b c4 00  3b 00 00 00 00 20 00 00  |...+....;.... ..|
 000004a0  01 00 00 00 09 a0 74 cc  00 93 00 00 00 00 2f 00  |......t......./.|
 000004b0  00 03 00 00 00 3f 48 ef  d6 00 5b 00 00 00 00 38  |.....?H...[....8|
 000004c0  00 00 02 00 00 00 f1 7e  7d dd 00 54 02 00 00 00  |.......~}..T....|
 000004d0  4f 00 00 0c 00 00 00 48  35 ff f5 00 c2 00 00 00  |O......H5.......|
 000004e0  00 3b 00 00 04 00 00 00  ad 61 4e ff 63 42 f7 73  |.;.......aN.cB.s|
 000004f0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
 *

 00000570  42 54 4c 46 00 09 00 16  00 00 00 00 25 00 00 00  |BTLF........%...|
 00000580  00 00 00 00 3b 00 00 00  00 20 00 00 01 00 00 00  |....;.... ......|
 00000590  00 5b 00 00 00 00 38 00  00 02 00 00 00 00 93 00  |.[....8.........|
 000005a0  00 00 00 2f 00 00 03 00  00 00 00 c2 00 00 00 00  |.../............|
 000005b0  3b 00 00 04 00 00 00 00  fd 00 00 00 00 25 00 00  |;............%..|
 000005c0  05 00 00 00 00 22 01 00  00 00 39 00 00 06 00 00  |....."....9.....|
 000005d0  00 00 5b 01 00 00 00 2d  00 00 07 00 00 00 00 88  |..[....-........|
 000005e0  01 00 00 00 33 00 00 08  00 00 00 00 bb 01 00 00  |....3...........|
 000005f0  00 46 00 00 09 00 00 00  00 01 02 00 00 00 21 00  |.F............!.|
 00000600  00 0a 00 00 00 00 22 02  00 00 00 32 00 00 0b 00  |......"....2....|
 00000610  00 00 00 54 02 00 00 00  4f 00 00 0c 00 00 00 00  |...T....O.......|
 00000620  a3 02 00 00 00 3e 00 00  0d 00 00 00 00 e1 02 00  |.....>..........|
 00000630  00 00 20 00 00 0e 00 00  00 00 01 03 00 00 00 28  |.. ............(|
 00000640  00 00 0f 00 00 00 00 29  03 00 00 00 1c 00 00 10  |.......)........|
 00000650  00 00 00 00 45 03 00 00  00 34 00 00 11 00 00 00  |....E....4......|
 00000660  00 79 03 00 00 00 35 00  00 12 00 00 00 00 ae 03  |.y....5.........|
 00000670  00 00 00 1b 00 00 13 00  00 00 00 c9 03 00 00 00  |................|
 00000680  26 00 00 14 00 00 00 00  16 04 00 00 00 2c 00 00  |&............,..|
 00000690  15 00 00 00 d3 c7 19 a0  00 00 00 00 00 00 00 00  |................|
 000006a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|

 */

static int readBTLF(struct READER *reader, struct BTREE *btree,
		int number_of_records, union RECORD *records) {

	int i;

	uint8_t type, message_flags;
	uint32_t creation_order, hash_of_name;
	uint64_t heap_id;

	char buf[4];

	UNUSED(heap_id);
	UNUSED(hash_of_name);
	UNUSED(creation_order);
	UNUSED(message_flags);

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "BTLF", 4)) {
		log("cannot read signature of BTLF\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	if (fgetc(reader->fhd) != 0) {
		log("object BTLF must have version 0\n");
		return MYSOFA_INVALID_FORMAT;
	}

	type = fgetc(reader->fhd);

	for (i = 0; i < number_of_records; i++) {

		switch (type) {
		case 5:
			records->type5.hash_of_name = readValue(reader, 4);
			records->type5.heap_id = readValue(reader, 7);
			log(" type5 %08X %14lX\n", records->type5.hash_of_name,
					records->type5.heap_id);
			records++;
			break;

		case 6:
			creation_order = readValue(reader, 8);
			heap_id = readValue(reader, 7);
			break;

		case 8:
			heap_id = readValue(reader, 8);
			message_flags = fgetc(reader->fhd);
			creation_order = readValue(reader, 4);
			hash_of_name = readValue(reader, 4);
			break;

		case 9:
			heap_id = readValue(reader, 8);
			message_flags = fgetc(reader->fhd);
			creation_order = readValue(reader, 4);
			break;

		default:
			log("object BTLF has unknown type %d\n", type);
			return MYSOFA_INVALID_FORMAT;
		}
	}

/*	fseeko(reader->fhd, bthd->root_node_address + bthd->node_size, SEEK_SET); skip checksum */

	return MYSOFA_OK;
}

/*  III.A.2. Disk Format: Level 1A2 - Version 2 B-trees

 000002d0  32 1d 42 54 48 44 00 08  00 02 00 00 11 00 00 00  |2.BTHD..........|
 000002e0  64 28 70 03 00 00 00 00  00 00 16 00 16 00 00 00  |d(p.............|
 000002f0  00 00 00 00 30 12 d9 6e  42 54 48 44 00 09 00 02  |....0..nBTHD....|
 00000300  00 00 0d 00 00 00 64 28  70 05 00 00 00 00 00 00  |......d(p.......|
 00000310  16 00 16 00 00 00 00 00  00 00 e2 0d 76 5c 46 53  |............v\FS|

 */

int btreeRead(struct READER *reader, struct BTREE *btree) {
	char buf[4];

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "BTHD", 4)) {
		log("cannot read signature of BTHD\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	if (fgetc(reader->fhd) != 0) {
		log("object BTHD must have version 0\n");
		return MYSOFA_INVALID_FORMAT;
	}

	btree->type = fgetc(reader->fhd);
	btree->node_size = readValue(reader, 4);
	btree->record_size = readValue(reader, 2);
	btree->depth = readValue(reader, 2);

	btree->split_percent = fgetc(reader->fhd);
	btree->merge_percent = fgetc(reader->fhd);
	btree->root_node_address = readValue(reader,
			reader->superblock.size_of_offsets);
	btree->number_of_records = readValue(reader, 2);
	btree->total_number = readValue(reader, reader->superblock.size_of_lengths);

	/*	fseek(reader->fhd, 4, SEEK_CUR);  skip checksum */

	btree->records = malloc(sizeof(btree->records[0]) * btree->total_number);
	if (!btree->records)
		return MYSOFA_NO_MEMORY;
	memset(btree->records, 0, sizeof(btree->records[0]) * btree->total_number);

	/* read records */
	fseek(reader->fhd, btree->root_node_address, SEEK_SET);
	return readBTLF(reader, btree, btree->number_of_records, btree->records);
}

void btreeFree(struct BTREE *btree) {
	free(btree->records);
}

/*  III.A.1. Disk Format: Level 1A1 - Version 1 B-trees
 *
 */

int treeRead(struct READER *reader, struct DATAOBJECT *data) {

	int i, j, err, olen, elements, size, x, y, z, b, e, dy, dz, sx, sy, sz, dzy,
			szy;
	char *input, *output;

	uint8_t node_type, node_level;
	uint16_t entries_used;
	uint32_t size_of_chunk;
	uint32_t filter_mask;
	uint64_t address_of_left_sibling, address_of_right_sibling, start[4],
			child_pointer, key, store;

	char buf[4];

	UNUSED(node_level);
	UNUSED(address_of_right_sibling);
	UNUSED(address_of_left_sibling);
	UNUSED(key);

	if (data->ds.dimensionality > 3) {
		log("TREE dimensions > 3");
		return MYSOFA_INVALID_FORMAT;
	}

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "TREE", 4)) {
		log("cannot read signature of TREE\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	node_type = fgetc(reader->fhd);
	node_level = fgetc(reader->fhd);
	entries_used = readValue(reader, 2);
	address_of_left_sibling = readValue(reader,
			reader->superblock.size_of_offsets);
	address_of_right_sibling = readValue(reader,
			reader->superblock.size_of_offsets);

	elements = 1;
	for (j = 0; j < data->ds.dimensionality; j++)
		elements *= data->datalayout_chunk[j];
	dy = data->datalayout_chunk[1];
	dz = data->datalayout_chunk[2];
	sx = data->ds.dimension_size[0];
	sy = data->ds.dimension_size[1];
	sz = data->ds.dimension_size[2];
	dzy = dz * dy;
	szy = sz * sy;
	size = data->datalayout_chunk[data->ds.dimensionality];

	log("elements %d size %d\n",elements,size);

	if (!(output = malloc(elements * size))) {
		return MYSOFA_NO_MEMORY;
	}

	for (e = 0; e < entries_used * 2; e++) {
		if (node_type == 0) {
			key = readValue(reader, reader->superblock.size_of_lengths);
		} else {
			size_of_chunk = readValue(reader, 4);
			filter_mask = readValue(reader, 4);
			if (filter_mask) {
				log("TREE all filters must be enabled\n");
				free(output);
				return MYSOFA_INVALID_FORMAT;
			}

			for (j = 0; j < data->ds.dimensionality; j++) {
				start[j] = readValue(reader, 8);
				log("start %d %lu\n",j,start[j]);
			}

			if (readValue(reader, 8)) {
				break;
			}

			child_pointer = readValue(reader,
					reader->superblock.size_of_offsets);
			log(" data at %lX len %u\n", child_pointer, size_of_chunk);

			/* read data */
			store = ftell(reader->fhd);
			if (fseek(reader->fhd, child_pointer, SEEK_SET)) {
				free(output);
				return errno;
			}

			if (!(input = malloc(size_of_chunk)))
				return MYSOFA_NO_MEMORY;
			if (fread(input, 1, size_of_chunk, reader->fhd) != size_of_chunk) {
				free(output);
				free(input);
				return MYSOFA_INVALID_FORMAT;
			}

			olen = elements * size;
			err = gunzip(size_of_chunk, input, &olen, output);
			free(input);

			log("   gunzip %d %d %d\n",err, olen, elements*size);
			if (err || olen != elements * size) {
				free(output);
				return MYSOFA_INVALID_FORMAT;
			}

			switch (data->ds.dimensionality) {
			case 1:
				for (i = 0; i < olen; i++) {
					b = i / elements;
					x = i % elements + start[0];
					if (x < sx) {
						j = x * size + b;
						((char*)data->data)[j] = output[i];
					}
				}
				break;
			case 2:
				for (i = 0; i < olen; i++) {
					b = i / elements;
					x = i % elements;
					y = x % dy + start[1];
					x = x / dy + start[0];
					if (y < sy && x < sx) {
						j = ((x * sy + y) * size) + b;
						((char*)data->data)[j] = output[i];
					}
				}
				break;
			case 3:
				for (i = 0; i < olen; i++) {
					b = i / elements;
					x = i % elements;
					z = x % dz + start[2];
					y = (x / dz) % dy + start[1];
					x = (x / dzy) + start[0];
					if (z < sz && y < sy && x < sx) {
						j = (x * szy + y * sz + z) * size + b;
						((char*)data->data)[j] = output[i];
					}
				}
				break;
			default:
				log("invalid dim\n");
				return MYSOFA_INTERNAL_ERROR;
			}

			fseek(reader->fhd, store, SEEK_SET);
		}
	}

	free(output);
	fseek(reader->fhd, 4, SEEK_CUR); /* skip checksum */

	return MYSOFA_OK;
}
