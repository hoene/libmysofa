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

#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include "reader.h"

int gunzip(int inlen, char *in, int *outlen, char *out) {
	int err;
	z_stream stream;

	memset(&stream, 0, sizeof(stream));
	stream.avail_in = inlen;
	stream.next_in = (unsigned char*) in;
	stream.avail_out = *outlen;
	stream.next_out = (unsigned char*) out;

	err = inflateInit(&stream);
	if (err)
		return err;

	err = inflate(&stream, Z_SYNC_FLUSH);
	*outlen = stream.total_out;
	inflateEnd(&stream);
	if (err && err != Z_STREAM_END) {
		log(" gunzip error %d %s\n",err,stream.msg);
		return err;
	}

	return MYSOFA_OK;
}
