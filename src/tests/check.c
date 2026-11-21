#include "../hrtf/tools.h"
#include "tests.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static void check(char *filename) {
  struct MYSOFA_HRTF *hrtf;
  int err;

  hrtf = mysofa_load(filename, &err);

  if (!hrtf) {
    CU_FAIL_FATAL("Error reading file.");
    return;
  }

  err = mysofa_check(hrtf);
  CU_ASSERT(err == MYSOFA_OK);

  mysofa_tocartesian(hrtf);

  CU_ASSERT(err == MYSOFA_OK);

  mysofa_free(hrtf);
}

void test_check() {
  check("tests/Pulse.sofa");
  check("tests/MIT_KEMAR_normal_pinna.old.sofa");
}

/* The file's Conventions attribute is "SOFX". The load refuses it after the
 * file's attributes have moved to the new profile, and must free them with
 * it: built with -DADDRESS_SANITIZE=ON, LeakSanitizer reports the leak. */
void test_check_conventions_not_sofa() {
  struct MYSOFA_HRTF *hrtf;
  int err = MYSOFA_OK;

  hrtf = mysofa_load("tests/fail-conventions-not-sofa.sofa", &err);
  CU_ASSERT(hrtf == NULL);
  CU_ASSERT(err == MYSOFA_INVALID_FORMAT);
  mysofa_free(hrtf);
}
