#include <assert.h>
#include <string.h>
#include <float.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "mysofa.h"

int main() {
	struct MYSOFA_EASY *easy;
	int err = 0;
	int filterlength;

	easy = mysofa_open("../../tests/sofa_api_mo_test/Pulse.sofa", 44100, &filterlength, &err);
	if (!easy)
		easy = mysofa_open("tests/sofa_api_mo_test/Pulse.sofa", 44100, &filterlength, &err);
	if (!easy) {
		fprintf(stderr, "Error reading file. Error code: %d\n", err);
		return err;
	}

	mysofa_close(easy);

	return 0;
}
