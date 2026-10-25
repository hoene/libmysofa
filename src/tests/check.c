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

void test_check_data_sizes() {
  struct MYSOFA_HRTF *hrtf;
  unsigned int elements;
  int err;

  hrtf = mysofa_load("tests/example_dummy_sofa48.sofa", &err);
  if (!hrtf) {
    CU_FAIL_FATAL("Error reading file.");
    return;
  }

  elements = hrtf->DataIR.elements;
  hrtf->DataIR.elements--;
  CU_ASSERT(mysofa_check(hrtf) == MYSOFA_INVALID_FORMAT);
  hrtf->DataIR.elements = elements;

  if (hrtf->DataDelay.values) {
    elements = hrtf->DataDelay.elements;
    hrtf->DataDelay.elements--;
    CU_ASSERT(mysofa_check(hrtf) == MYSOFA_INVALID_FORMAT);
    hrtf->DataDelay.elements = elements;
  }

  hrtf->N = 0;
  CU_ASSERT(mysofa_check(hrtf) == MYSOFA_INVALID_DIMENSIONS);

  mysofa_free(hrtf);
}
