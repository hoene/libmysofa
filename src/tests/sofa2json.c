/*

  Copyright 2016 Christian Hoene, Symonics GmbH

*/

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "json.h"

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

	printJson(stdout,hrtf);

	mysofa_free(hrtf);

	return 0;
}
