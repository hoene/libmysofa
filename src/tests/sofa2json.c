/*

 Copyright 2016 Christian Hoene, Symonics GmbH

 */

#include "../hrtf/mysofa.h"
#include "../hrtf/tools.h"
#include "json.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  struct MYSOFA_HRTF *hrtf = NULL;
  int err = 0;
  int sanitize = 0;
  char *filename;

  if (argc == 3 && !strcmp("-s", argv[1])) {
    sanitize = 1;
    filename = argv[2];
  } else if (argc == 2) {
    filename = argv[1];
  } else {
    fprintf(stderr,
            "Usage: %s [-s] <FILE.SOFA>\n converts a sofa file to json "
            "output.\nAdd -s to sanitize the json output from netcdf fields.\n",
            argv[0]);
    return 1;
  }

  hrtf = mysofa_load(filename, &err);

  if (!hrtf) {
    fprintf(stderr, "Error reading file %s. Error code: %d\n", argv[1], err);
    return err;
  }

  printJson(stdout, hrtf, sanitize);

  mysofa_free(hrtf);

  return 0;
}
