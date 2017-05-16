/*

 Copyright 2016 Christian Hoene, Symonics GmbH

 */

/* IV.A.1.b. Version 2 Data Object Header Prefix

 00000030  4f 48 44 52 02 2d d3 18  2b 53 d3 18 2b 53 d3 18  |OHDR.-..+S..+S..|
 00000040  2b 53 d3 18 2b 53 f4 01  02 22 00 00 00 00        |+S..+S..."......|
 ....
 00000230  00 00 00 00 00 00 00 00  00 00 00 00 f9 ba 5d c9  |..............].|
 */

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>
#include <errno.h>
#include "reader.h"

static int readOCHK(struct READER *reader, struct DATAOBJECT *dataobject,
		uint64_t end);

static struct DATAOBJECT *findDataobject(struct READER *reader,
		uint64_t address) {
	struct DATAOBJECT *p = reader->all;
	while (p && p->address != address)
		p = p->all;

	return p;
}

/*
 * IV.A.2.a. The NIL Message

 00000090                                 00 9c 01 00 00 00  |................|
 000000a0  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
 *
 00000230  00 00 00 00 00 00 00 00  00 00 00 00 f9 ba 5d c9  |..............].|

 */

static int readOHDRHeaderMessageNIL(struct READER *reader, int length) {

	if (fseek(reader->fhd, length, SEEK_CUR))
		return errno;

	return MYSOFA_OK;
}

/*
 * IV.A.2.b. The Dataspace Message

 */

static int readOHDRHeaderMessageDataspace(struct READER *reader,
		struct DATASPACE *ds) {

	int i;

	if (fgetc(reader->fhd) != 2) {
		log("object OHDR dataspace message must have version 2\n");
		return MYSOFA_INVALID_FORMAT;
	}

	ds->dimensionality = fgetc(reader->fhd);
	if (ds->dimensionality > 4) {
		log("dimensionality must be low 5\n");
		return MYSOFA_INVALID_FORMAT;
	}

	ds->flags = fgetc(reader->fhd);
	ds->type = fgetc(reader->fhd);

	for (i = 0; i < ds->dimensionality; i++) {
		if (i < 4) {
			ds->dimension_size[i] = readValue(reader,
					reader->superblock.size_of_lengths);
			log("   dimension %d %lu\n", i, ds->dimension_size[i]);
		} else
			readValue(reader, reader->superblock.size_of_lengths);
	}

	if (ds->flags & 1) {
		for (i = 0; i < ds->dimensionality; i++) {
			if (i < 4)
				ds->dimension_max_size[i] = readValue(reader,
						reader->superblock.size_of_lengths);
			else
				readValue(reader, reader->superblock.size_of_lengths);

		}
	}

	return MYSOFA_OK;
}

/*
 * IV.A.2.c. The Link Info Message

 00 03  |+S..+S..."......|
 00000050  0f 00 00 00 00 00 00 00  c9 11 00 00 00 00 00 00  |................|
 00000060  5b 12 00 00 00 00 00 00  81 12 00 00 00 00 00 00  |[...............|
 */

static int readOHDRHeaderMessageLinkInfo(struct READER *reader,
		struct LINKINFO *li) {

	if (fgetc(reader->fhd) != 0) {
		log("object OHDR link info message must have version 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	li->flags = fgetc(reader->fhd);

	if (li->flags & 1)
		li->maximum_creation_index = readValue(reader, 8);

	li->fractal_heap_address = readValue(reader,
			reader->superblock.size_of_offsets);
	li->address_btree_index = readValue(reader,
			reader->superblock.size_of_offsets);

	if (li->flags & 2)
		li->address_btree_order = readValue(reader,
				reader->superblock.size_of_offsets);

	return MYSOFA_OK;
}

/*
 * IV.A.2.d. The Datatype Message

 000007c0                       03  14 00 01 00 00|11|21 1f  |..............!.|
 000007d0  00|04 00 00 00|00 00|20  00|17|08|00|17|7f 00 00  |....... ........|
 000007e0  00|05 02 00 01 00 00 03  0a 10 10 00 00 07 00 6d  |...............m|
 000007f0  36 00 00 00 00 00 00 ea  00 00 00 00 00 00 00 15  |6...............|

 */

static int readOHDRHeaderMessageDatatype(struct READER *reader,
		struct DATATYPE *dt) {

	int i, j, c, err;
	char *buffer, *p;
	struct DATATYPE dt2;

	dt->class_and_version = fgetc(reader->fhd);
	if ((dt->class_and_version & 0xf0) != 0x10
			&& (dt->class_and_version & 0xf0) != 0x30) {
		log("object OHDR datatype message must have version 1 not %d\n",
				dt->class_and_version >> 4);
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	dt->class_bit_field = readValue(reader, 3);
	dt->size = readValue(reader, 4);

	switch (dt->class_and_version & 0xf) {
	case 0: /* int */
		dt->u.i.bit_offset = readValue(reader, 2);
		dt->u.i.bit_precision = readValue(reader, 2);
		log("    INT bit %d %d\n", dt->u.i.bit_offset, dt->u.i.bit_precision);
		break;

	case 1: /* float */
		dt->u.f.bit_offset = readValue(reader, 2);
		dt->u.f.bit_precision = readValue(reader, 2);
		dt->u.f.exponent_location = fgetc(reader->fhd);
		dt->u.f.exponent_size = fgetc(reader->fhd);
		dt->u.f.mantissa_location = fgetc(reader->fhd);
		dt->u.f.mantissa_size = fgetc(reader->fhd);
		dt->u.f.exponent_bias = readValue(reader, 4);

		log("    FLOAT bit %d %d exponent %d %d MANTISSA %d %d OFFSET %d\n",
				dt->u.f.bit_offset, dt->u.f.bit_precision, dt->u.f.exponent_location,
				dt->u.f.exponent_size, dt->u.f.mantissa_location,
				dt->u.f.mantissa_size, dt->u.f.exponent_bias);

		/* FLOAT bit 0 32 exponent 23 8 MANTISSA 0 23 OFFSET 127    				
		 FLOAT bit 0 64 exponent 52 11 MANTISSA 0 52 OFFSET 1023 */

		if (dt->u.f.bit_offset != 0 || dt->u.f.mantissa_location != 0
				|| (dt->u.f.bit_precision != 32 && dt->u.f.bit_precision != 64)
				|| (dt->u.f.bit_precision == 32
						&& (dt->u.f.exponent_location != 23
								|| dt->u.f.exponent_size != 8
								|| dt->u.f.mantissa_size != 23
								|| dt->u.f.exponent_bias != 127))
				|| (dt->u.f.bit_precision == 64
						&& (dt->u.f.exponent_location != 52
								|| dt->u.f.exponent_size != 11
								|| dt->u.f.mantissa_size != 52
								|| dt->u.f.exponent_bias != 1023)))
			return MYSOFA_UNSUPPORTED_FORMAT;
		break;

	case 3: /* string */
		log("    STRING %d %02X\n", dt->size, dt->class_bit_field);
		break;

	case 6:
		log("    COMPONENT %d %02X\n", dt->size, dt->class_bit_field);
		if ((dt->class_and_version & 0xf0) != 0x30) {
			log("object OHDR datatype message must have version 1 not %d\n",
					dt->class_and_version >> 4);
			return MYSOFA_INVALID_FORMAT;
		}
		for (i = 0; i < (dt->class_bit_field & 0xffff); i++) {
			buffer = malloc(64);
			j = 0;
			do {
				if (j > 64) {
					p = realloc(buffer, j);
					if (!p) {
						free(buffer);
						return err;
					}
					buffer = p;
				}
				c = fgetc(reader->fhd);
				buffer[j] = c;
				j++;
			} while (c > 0);

			for (j = 0, c = 0; (dt->size >> (8 * j)) > 0; j++) {
				c |= fgetc(reader->fhd) << (8 * j);
			} log("   COMPONENT %s offset %d\n", buffer, c);
			free(buffer);
			err = readOHDRHeaderMessageDatatype(reader, &dt2);
			if (err)
				return err;
		}
		break;

	case 7: /* reference */
		log("   REFERENCE %d %02X\n", dt->size, dt->class_bit_field);
		break;

	case 9: /* list */
		dt->list = dt->size;
		log("  LIST %d\n", dt->size);
		err = readOHDRHeaderMessageDatatype(reader, dt);
		if (err)
			return err;
		break;

	default:
		log("object OHDR datatype message has unknown variable type %d\n",
				dt->class_and_version & 0xf);
		return MYSOFA_UNSUPPORTED_FORMAT;

	}
	return MYSOFA_OK;
}

/*
 * IV.A.2.f. The Data Storage - Fill Value Message

 000007e0    |05 02 00 01 00 00|03  0a

 */

static int readOHDRHeaderMessageDataFill(struct READER *reader) {

	uint8_t version;
	uint8_t flags;
	uint32_t size;

	version = fgetc(reader->fhd);
	if (version != 3) {
		log(
				"object OHDR data storage fill value message must have version 3 not %d\n",
				version);
		return MYSOFA_INVALID_FORMAT;
	}

	flags = fgetc(reader->fhd);

	if (flags & (1 << 5)) {
		size = readValue(reader, 4);
		if (fseek(reader->fhd, size, SEEK_CUR))
			return errno;
	}

	return MYSOFA_OK;
}

/*
 * IV.A.2.i. The Data Layout Message

 00000ec0                       08  00 00 00 00 00 00 00 00  |......+.........|
 00000ed0  00 9e 47 0b 16 00 01 00  00 02 02 02 00 01 00 01  |..G.............|
 00000ee0  00 08 00 00 00 01 00 01  00 01 00 01 00 00 00 08  |................|
 00000ef0  17 00 01 00 00 03 02 03  01 42 00 00 00 00 00 00  |.........B......|
 00000f00  01 00 00 00 03 00 00 00  08 00 00 00 15 1c 00 04  |................|


 03 02 03  01 42 00 00 00 00 00 00  |.........B......|
 00000f00  01 00 00 00 03 00 00 00  08 00 00 00 15 1c 00 04  |................|
 00000f10  00 00 00 03 03 00 ff ff  ff ff ff ff ff ff ff ff  |................|
 00000f20  ff ff ff ff ff ff ff ff  ff ff ff ff ff ff 0c 23  |...............#|
 00000f30  00 00 00 00 03 00 05 00  08 00 04 00 00 54 79 70  |.............Typ|


 */

static int readOHDRHeaderMessageDataLayout(struct READER *reader,
		struct DATAOBJECT *data) {

	int i, err;
	unsigned size;

	uint8_t dimensionality, layout_class;
	uint32_t dataset_element_size;
	uint64_t data_address, store, data_size;

	UNUSED(dataset_element_size);
	UNUSED(data_size);

	if (fgetc(reader->fhd) != 3) {
		log("object OHDR message data layout message must have version 3\n");
		return MYSOFA_INVALID_FORMAT;
	}

	layout_class = fgetc(reader->fhd);

	switch (layout_class) {
#if 0
	case 0:
	data_size = readValue(reader, 2);
	fseek(reader->fhd, data_size, SEEK_CUR);
	log("TODO 0 SIZE %u\n", data_size);
	break;
#endif
	case 1:
		data_address = readValue(reader, reader->superblock.size_of_offsets);
		data_size = readValue(reader, reader->superblock.size_of_lengths);
		log("TODO 1 SIZE %lu\n", data_size);
		break;

	case 2:
		dimensionality = fgetc(reader->fhd);
		data_address = readValue(reader, reader->superblock.size_of_offsets);
		log(" CHUNK %lX\n", data_address);
		for (i = 0; i < dimensionality; i++) {
			data->datalayout_chunk[i] = readValue(reader, 4);
			log(" %d\n", data->datalayout_chunk[i]);
		}
		/* TODO last entry? error in spec: ?*/

		size = data->datalayout_chunk[dimensionality - 1];
		for (i = 0; i < data->ds.dimensionality; i++)
			size *= data->ds.dimension_size[i];

		if (validAddress(reader, data_address)) {
			store = ftell(reader->fhd);
			if (fseek(reader->fhd, data_address, SEEK_SET))
				return errno;
			if (!data->data) {
				data->data_len = size;
				data->data = malloc(size);
				if (!data->data)
					return MYSOFA_NO_MEMORY;
			}
			err = treeRead(reader, data);
			if (err)
				return err;
			if (fseek(reader->fhd, store, SEEK_SET))
				return errno;
		}
		break;

	default:
		log(
				"object OHDR message data layout message has unknown layout class %d\n",
				layout_class);
		return MYSOFA_INVALID_FORMAT;

	}

	return MYSOFA_OK;
}

/*
 * IV.A.2.k. The Group Info Message

 *  00000070  0a 02 00 01 00 00 00 00
 *
 */

static int readOHDRHeaderMessageGroupInfo(struct READER *reader,
		struct GROUPINFO *gi) {

	if (fgetc(reader->fhd) != 0) {
		log("object OHDR group info message must have version 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	gi->flags = fgetc(reader->fhd);

	if (gi->flags & 1) {
		gi->maximum_compact_value = readValue(reader, 2);
		gi->minimum_dense_value = readValue(reader, 2);
	}

	if (gi->flags & 2) {
		gi->number_of_entries = readValue(reader, 2);
		gi->length_of_entries = readValue(reader, 2);
	}
	return MYSOFA_OK;
}

/*
 * IV.A.2.l. The Data Storage - Filter Pipeline Message
 *
 *  00000070  0a 02 00 01 00 00 00 00
 *
 */

static int readOHDRHeaderMessageFilterPipeline(struct READER *reader) {
	int i, j;
	uint8_t filters;
	uint16_t filter_identification_value, flags, number_client_data_values;
	uint32_t client_data;
	uint64_t maximum_compact_value, minimum_dense_value, number_of_entries,
			length_of_entries;

	UNUSED(flags);
	UNUSED(client_data);
	UNUSED(maximum_compact_value);
	UNUSED(minimum_dense_value);
	UNUSED(number_of_entries);
	UNUSED(length_of_entries);

	if (fgetc(reader->fhd) != 2) {
		log("object OHDR filter pipeline message must have version 1\n");
		return MYSOFA_INVALID_FORMAT;
	}

	filters = fgetc(reader->fhd);
	if (filters > 32) {
		log("object OHDR filter pipeline message has too many filters: %d\n",
				filters);
		return MYSOFA_INVALID_FORMAT;
	}

	for (i = 0; i < filters; i++) {
		filter_identification_value = readValue(reader, 2);
		switch (filter_identification_value) {
		case 1:
		case 2:
			break;
		default:
			log(
					"object OHDR filter pipeline message contains unsupported filter: %d\n",
					filter_identification_value);
			return MYSOFA_INVALID_FORMAT;
		} log("  filter %d\n", filter_identification_value);
		flags = readValue(reader, 2);
		number_client_data_values = readValue(reader, 2);
		/* no name here */
		for (j = 0; j < number_client_data_values; j++) {
			client_data = readValue(reader, 4);
		}

	}

	return MYSOFA_OK;
}

int readDataVar(struct READER *reader, struct DATAOBJECT *data,
		struct DATATYPE *dt, struct DATASPACE *ds) {

	char *buffer, number[16];
	uint64_t reference, gcol, dataobject;
	int err;
	struct DATAOBJECT *referenceData;

	if (dt->list) {
		if (dt->list - dt->size == 8) {
			readValue(reader, 4);       /* TODO unknown? */
			gcol = readValue(reader, 4);
		} else {
			gcol = readValue(reader, dt->list - dt->size);
		} log("    GCOL %d %8lX %8lX\n",dt->list - dt->size,gcol,ftell(reader->fhd));
/*		fseek(reader->fhd, dt->list - dt->size, SEEK_CUR); TODO: missing part in specification */
	}

	switch (dt->class_and_version & 0xf) {
	case 3:
		buffer = malloc(dt->size + 1);
		if (buffer == NULL) {
			log("data reader no memory\n");
			return MYSOFA_NO_MEMORY;
		}
		if (fread(buffer, 1, dt->size, reader->fhd) != dt->size) {
			free(buffer);
			log("data reader cannot read\n");
			return MYSOFA_READ_ERROR;
		}
		buffer[dt->size] = 0;
		log("STRING %s\n", buffer);
		data->string = buffer;
		break;

		/*
		 * 000036e3   67 0e 00 00 00  00 00 00 00 00 00 00 00  |...g............|
		 000036f0  00 00 00
		 */
	case 6:
		/* TODO unclear spec */
		log("COMPONENT todo %lX %d\n", ftell(reader->fhd),dt->size);
		if (fseek(reader->fhd, dt->size, SEEK_CUR))
			return errno;
		break;

	case 7:
		readValue(reader, 4);       /* TODO unclear reference */
		reference = readValue(reader, dt->size - 4);
		log(" REFERENCE size %d %lX\n",dt->size, reference);
		if (!!(err = gcolRead(reader, gcol, reference, &dataobject))) {
			return MYSOFA_OK;       /* ignore error
            return err; */
		}
		referenceData = findDataobject(reader, dataobject);
		if (referenceData)
			buffer = referenceData->name;
		else {
			sprintf(number, "REF%08lX", reference);
			buffer = number;
		}
		log("    REFERENCE %lu %lX %s\n", reference, dataobject, buffer);
/*		if(!referenceData) {
		    return MYSOFA_UNSUPPORTED_FORMAT;
		} */
		if (data->string) {
			data->string = realloc(data->string,
					strlen(data->string) + strlen(buffer) + 2);
			strcat(data->string, ",");
			strcat(data->string, buffer);
		} else {
			data->string = mysofa_strdup(buffer);
		}
		break;

	default:
		log("data reader unknown type %d\n", dt->class_and_version & 0xf);
		return MYSOFA_INTERNAL_ERROR;
	}
	return MYSOFA_OK;
}

int readDataDim(struct READER *reader, struct DATAOBJECT *da,
		struct DATATYPE *dt, struct DATASPACE *ds, int dim) {
	int i, err;

	for (i = 0; i < ds->dimension_size[dim]; i++) {
		if (dim + 1 < ds->dimensionality) {
			if (!!(err = readDataDim(reader, da, dt, ds, dim + 1))) {
				return err;
			}
		} else {
			if (!!(err = readDataVar(reader, da, dt, ds))) {
				return err;
			}
		}
	}
	return MYSOFA_OK;
}

int readData(struct READER *reader, struct DATAOBJECT *da, struct DATATYPE *dt,
		struct DATASPACE *ds) {
	if (ds->dimensionality == 0) {
		ds->dimension_size[0] = 1;
	}
	return readDataDim(reader, da, dt, ds, 0);
}

/*
 IV.A.2.q. The Object Header Continuation Message

 10 10 00 00 07 00 6d  |...............m|
 000007f0  36 00 00 00 00 00 00 ea  00 00 00 00 00 00 00 15  |6...............|
 */

static int readOHDRHeaderMessageContinue(struct READER *reader,
		struct DATAOBJECT *dataobject) {

	int err;
	uint64_t offset, length, store;

	offset = readValue(reader, reader->superblock.size_of_offsets);
	length = readValue(reader, reader->superblock.size_of_lengths);
	log(" continue %08lX %08lX\n", offset, length);

	store = ftell(reader->fhd);
	fseek(reader->fhd, offset, SEEK_SET);

	err = readOCHK(reader, dataobject, offset + length);
	if (err)
		return err;

	fseek(reader->fhd, store, SEEK_SET);
	log(" continue back\n");
	return MYSOFA_OK;
}

/*
 IV.A.2.m. The Attribute Message

 */

static int readOHDRHeaderMessageAttribute(struct READER *reader,
		struct DATAOBJECT *dataobject) {
	int err;

	uint8_t flags, encoding;
	uint16_t name_size, datatype_size, dataspace_size;
	char *name;
	struct DATAOBJECT d;
	struct MYSOFA_ATTRIBUTE *attr;

	UNUSED(encoding);
	UNUSED(datatype_size);
	UNUSED(dataspace_size);

	memset(&d, 0, sizeof(d));

	if (fgetc(reader->fhd) != 3) {
		log("object OHDR attribute message must have version 3\n");
		return MYSOFA_INVALID_FORMAT;
	}

	flags = fgetc(reader->fhd);

	name_size = readValue(reader, 2);
	datatype_size = readValue(reader, 2);
	dataspace_size = readValue(reader, 2);
	encoding = fgetc(reader->fhd);

	name = malloc(name_size);
	fread(name, 1, name_size, reader->fhd);
	log("  attribute name %s\n", name);

	if (flags & 3) {
		log("object OHDR attribute message must have any flags set\n");
		free(name);
		return MYSOFA_INVALID_FORMAT;
	}
	err = readOHDRHeaderMessageDatatype(reader, &d.dt);
	if (err) {
		log("object OHDR attribute message read datatype error\n");
		free(name);
		return MYSOFA_INVALID_FORMAT;
	}
	err = readOHDRHeaderMessageDataspace(reader, &d.ds);
	if (err) {
		log("object OHDR attribute message read dataspace error\n");
		free(name);
		return MYSOFA_INVALID_FORMAT;
	}
	err = readData(reader, &d, &d.dt, &d.ds);
	if (err) {
		log("object OHDR attribute message read data error\n");
		free(name);
		return MYSOFA_INVALID_FORMAT;
	}

	attr = malloc(sizeof(struct MYSOFA_ATTRIBUTE));
	attr->name = name;
	attr->value = d.string;
	d.string = NULL;
	attr->next = dataobject->attributes;
	dataobject->attributes = attr;

	dataobjectFree(reader, &d);
	return MYSOFA_OK;
}

/*
 * IV.A.2.v. The Attribute Info Message

 00000070                           15 1c 00 04 00 00 00 03  |................|
 00000080  16 00 40 02 00 00 00 00  00 00 d2 02 00 00 00 00  |..@.............|
 00000090  00 00 f8 02 00 00 00 00  00 00

 */

static int readOHDRHeaderMessageAttributeInfo(struct READER *reader,
		struct ATTRIBUTEINFO *ai) {

	if (fgetc(reader->fhd) != 0) {
		log("object OHDR attribute info message must have version 0\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	ai->flags = fgetc(reader->fhd);

	if (ai->flags & 1)
		ai->maximum_creation_index = readValue(reader, 2);

	ai->fractal_heap_address = readValue(reader,
			reader->superblock.size_of_offsets);
	ai->attribute_name_btree = readValue(reader,
			reader->superblock.size_of_offsets);

	if (ai->flags & 2)
		ai->attribute_creation_order_btree = readValue(reader,
				reader->superblock.size_of_offsets);

	return MYSOFA_OK;
}

/**
 * read all OHDR messages
 */
static int readOHDRmessages(struct READER *reader,
		struct DATAOBJECT *dataobject, uint64_t end_of_messages) {

	FILE *fhd = reader->fhd;
	int err;
	long end;

	while (ftell(fhd) < end_of_messages - 4) { /* final gap may has a size of up to 3 */
		uint8_t header_message_type = fgetc(fhd);
		uint16_t header_message_size = readValue(reader, 2);
		uint8_t header_message_flags = fgetc(fhd);
		if ((header_message_flags & ~5) != 0) {
			log("OHDR unsupported OHDR message flag %02X\n",
					header_message_flags);
			return MYSOFA_UNSUPPORTED_FORMAT;
		}

		if ((dataobject->flags & (1 << 2)) != 0)
			/* ignore header_creation_order */
			fseek(reader->fhd, 2, SEEK_CUR);

		log(" OHDR message type %2d offset %6lX len %4X\n", header_message_type,
				ftell(fhd), header_message_size);

		end = ftell(fhd) + header_message_size;

		switch (header_message_type) {
		case 0: /* NIL Message */
			if (!!(err = readOHDRHeaderMessageNIL(reader, header_message_size)))
				return err;
			break;
		case 1: /* Dataspace Message */
			if (!!(err = readOHDRHeaderMessageDataspace(reader, &dataobject->ds)))
				return err;
			break;
		case 2: /* Link Info Message */
			if (!!(err = readOHDRHeaderMessageLinkInfo(reader, &dataobject->li)))
				return err;
			break;
		case 3: /* Datatype Message */
			if (!!(err = readOHDRHeaderMessageDatatype(reader, &dataobject->dt)))
				return err;
			break;
		case 5: /* Data Fill Message */
			if (!!(err = readOHDRHeaderMessageDataFill(reader)))
				return err;
			break;
		case 8: /* Data Layout Message */
			if (!!(err = readOHDRHeaderMessageDataLayout(reader, dataobject)))
				return err;
			break;
		case 10: /* Group Info Message */
			if (!!(err = readOHDRHeaderMessageGroupInfo(reader, &dataobject->gi)))
				return err;
			break;
		case 11: /* Filter Pipeline Message */
			if (!!(err = readOHDRHeaderMessageFilterPipeline(reader)))
				return err;
			break;
		case 12: /* Attribute Message */
			if (!!(err = readOHDRHeaderMessageAttribute(reader, dataobject)))
				return err;
			break;
		case 16: /* Continue Message */
			if (!!(err = readOHDRHeaderMessageContinue(reader, dataobject)))
				return err;
			break;
		case 21: /* Attribute Info Message */
			if (!!(err = readOHDRHeaderMessageAttributeInfo(reader,
					&dataobject->ai)))
				return err;
			break;
		default:
			log("OHDR unknown header message of type %d\n",
					header_message_type);

			return MYSOFA_UNSUPPORTED_FORMAT;
		}

		if (ftell(fhd) != end) {
			log("OHDR message length mismatch by %ld\n", ftell(fhd) - end);
			return MYSOFA_INTERNAL_ERROR;
		}
	}

	fseek(fhd, end_of_messages + 4, SEEK_SET); /* skip checksum */

	return MYSOFA_OK;
}

static int readOCHK(struct READER *reader, struct DATAOBJECT *dataobject,
		uint64_t end) {
	int err;
	char buf[4];

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "OCHK", 4)) {
		log("cannot read signature of OCHK\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", (uint64_t )ftell(reader->fhd) - 4, buf);

	err = readOHDRmessages(reader, dataobject, end - 4); /* substract checksum */
	if (err) {
		return err;
	}

	return MYSOFA_OK;
}

int dataobjectRead(struct READER *reader, struct DATAOBJECT *dataobject,
		char *name) {
	uint64_t size_of_chunk, end_of_messages;
	int err;
	char buf[4];

	memset(dataobject, 0, sizeof(*dataobject));
	dataobject->address = ftell(reader->fhd);
	dataobject->name = name;

	/* read signature */
	if (fread(buf, 1, 4, reader->fhd) != 4 || strncmp(buf, "OHDR", 4)) {
		log("cannot read signature of data object\n");
		return MYSOFA_INVALID_FORMAT;
	} log("%08lX %.4s\n", dataobject->address, buf);

	if (fgetc(reader->fhd) != 2) {
		log("object OHDR must have version 2\n");
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	dataobject->flags = fgetc(reader->fhd);

	if (dataobject->flags & (1 << 5)) { /* bit 5 indicated time stamps */
		fseek(reader->fhd, 16, SEEK_CUR); /* skip them */
	}

	if (dataobject->flags & (1 << 4)) { /* bit 4 ? */
		log("OHDR: unsupported flags bit 4: %02X\n", dataobject->flags);
		return MYSOFA_UNSUPPORTED_FORMAT;
	}

	size_of_chunk = readValue(reader, 1 << (dataobject->flags & 3));
	end_of_messages = ftell(reader->fhd) + size_of_chunk;

	err = readOHDRmessages(reader, dataobject, end_of_messages);

	if (err) {
		return err;
	}

	/* not needed
	 if (validAddress(reader, dataobject->ai.attribute_name_btree)) {
	 fseek(reader->fhd, dataobject->ai.attribute_name_btree, SEEK_SET);
	 btreeRead(reader, &dataobject->attributes);
	 }
	 */

	/* parse message attribute info */
	if (validAddress(reader, dataobject->ai.fractal_heap_address)) {
		fseek(reader->fhd, dataobject->ai.fractal_heap_address, SEEK_SET);
		fractalheapRead(reader, dataobject, &dataobject->attributes_heap);
	}

	/* parse message link info */
	if (validAddress(reader, dataobject->li.fractal_heap_address)) {
		fseek(reader->fhd, dataobject->li.fractal_heap_address, SEEK_SET);
		err = fractalheapRead(reader, dataobject, &dataobject->objects_heap);
		if (err)
			return err;
	}

	/* no needed 
	 if (validAddress(reader, dataobject->li.address_btree_index)) {
	 fseek(reader->fhd, dataobject->li.address_btree_index, SEEK_SET);
	 btreeRead(reader, &dataobject->objects);
	 }
	 */

	dataobject->all = reader->all;
	reader->all = dataobject;

	return MYSOFA_OK;
}

void dataobjectFree(struct READER *reader, struct DATAOBJECT *dataobject) {
    struct DATAOBJECT **p;
    
	btreeFree(&dataobject->attributes_btree);
	fractalheapFree(&dataobject->attributes_heap);
	btreeFree(&dataobject->objects_btree);
	fractalheapFree(&dataobject->objects_heap);

	while (dataobject->attributes) {
		struct MYSOFA_ATTRIBUTE *attr = dataobject->attributes;
		dataobject->attributes = attr->next;
		free(attr->name);
		free(attr->value);
		free(attr);
	}

	while (dataobject->directory) {
		struct DIR *dir = dataobject->directory;
		dataobject->directory = dir->next;
		dataobjectFree(reader, &dir->dataobject);
		free(dir);
	}

	free(dataobject->data);
	free(dataobject->string);
	free(dataobject->name);

	p = &reader->all;
	while (*p) {
		if ((*p) == dataobject) {
			*p = dataobject->all;
			break;
		}
		p = &((*p)->all);
	}
}

