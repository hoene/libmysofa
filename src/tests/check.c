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

/* Each file is tests/testfile.sofa with one declared size made larger than
 * the file can hold: an attribute name in a fractal heap, a chunk, and two
 * chunked datasets that would inflate to more than the whole file can. The
 * load must refuse each file before it allocates a buffer of that size. */
void test_check_sizes_past_end() {
  static const char *files[] = {"tests/fail-heap-name-past-end.sofa",
                                "tests/fail-chunk-past-end.sofa",
                                "tests/fail-chunk-inflates-past-file.sofa",
                                "tests/fail-dataset-inflates-past-file.sofa"};
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
