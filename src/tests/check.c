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

/* Each file is tests/testfile.sofa with one size in a fractal heap's header
 * set to 0. Each of these sizes goes into a logarithm: the load must refuse
 * the file, not take log2(0). The file with a starting block size of 0 used
 * to load. */
void test_check_zero_heap_sizes() {
  static const char *files[] = {
      "tests/fail-heap-zero-starting-block-size.sofa",
      "tests/fail-heap-zero-maximum-direct-block-size.sofa",
      "tests/fail-heap-zero-maximum-heap-size.sofa",
      "tests/fail-heap-zero-maximum-size.sofa"};
  struct MYSOFA_HRTF *hrtf;
  unsigned int i;
  int err;

  for (i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    err = MYSOFA_OK;
    hrtf = mysofa_load(files[i], &err);
    CU_ASSERT(hrtf == NULL);
    CU_ASSERT(err == MYSOFA_INVALID_FORMAT);
    mysofa_free(hrtf);
  }
}
