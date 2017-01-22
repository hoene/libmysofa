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

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"

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

	json(hrtf);

	mysofa_free(hrtf);

	return 0;
}
