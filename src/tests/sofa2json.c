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
#include <unistd.h>

static void usage(const char *appname) {
  fprintf(stderr,
          "Usage: %s [-s] [-c] <FILE.SOFA>\n converts a sofa file to json "
          "output.\nAdd -s to sanitize the json output from netcdf fields.\n"
          "Add -c to do libmysofa checking of the HRTF format.\n",
          appname);
  exit(EXIT_FAILURE);
}

static const char *getErrorText(int err) {
  const char *result = NULL;
  switch (err) {
  case MYSOFA_OK:
    break;
  case MYSOFA_INTERNAL_ERROR:
    result = "internal error";
    break;
  case MYSOFA_INVALID_FORMAT:
    result = "invalid format";
    break;
  case MYSOFA_UNSUPPORTED_FORMAT:
    result = "unsupported format";
    break;
  case MYSOFA_NO_MEMORY:
    result = "not enough memory";
    break;
  case MYSOFA_READ_ERROR:
    result = "read error";
    break;
  case MYSOFA_INVALID_ATTRIBUTES:
    result = "invalid attributes";
    break;
  case MYSOFA_INVALID_DIMENSIONS:
    result = "invalid dimensions";
    break;
  case MYSOFA_INVALID_DIMENSION_LIST:
    result = "invalid dimension list";
    break;
  case MYSOFA_INVALID_COORDINATE_TYPE:
    result = "invalid coordinate type";
    break;
  case MYSOFA_ONLY_EMITTER_WITH_ECI_SUPPORTED:
    result = "only emtter with ECI dimensions supported";
    break;
  case MYSOFA_ONLY_DELAYS_WITH_IR_OR_MR_SUPPORTED:
    result = "only delays with IR or MR dimensions spported";
    break;
  case MYSOFA_ONLY_THE_SAME_SAMPLING_RATE_SUPPORTED:
    result = "only the same sampling rate for all supported";
    break;
  case MYSOFA_RECEIVERS_WITH_RCI_SUPPORTED:
    result = "only receives with RCI dimensions supported";
    break;
  case MYSOFA_RECEIVERS_WITH_CARTESIAN_SUPPORTED:
    result = "only receivers with cartesian coordinate system supported";
    break;
  case MYSOFA_INVALID_RECEIVER_POSITIONS:
    result = "invalid receiver positions";
    break;
  case MYSOFA_ONLY_SOURCES_WITH_MC_SUPPORTED:
    result = "only audio sources with MC dimensions supported";
    break;
  default:
    result = strerror(err);
  }
  return result;
}

int main(int argc, char **argv) {
  struct MYSOFA_HRTF *hrtf = NULL;
  int err = 0;
  int sanitize = 0;
  int check = 0;
  int opt;
  char *filename;

  while ((opt = getopt(argc, argv, "sc")) != -1) {
    switch (opt) {
    case 's':
      sanitize = 1;
      break;
    case 'c':
      check = 1;
      break;
    default:
      usage(argv[0]);
    }
  }

  if (optind >= argc)
    usage(argv[0]);

  filename = argv[optind];

  hrtf = mysofa_load(filename, &err);
  if (!hrtf || err != MYSOFA_OK) {
    fprintf(stderr, "Error reading file %s. Error code: %d:%s\n", filename, err,
            getErrorText(err));
    exit(EXIT_FAILURE);
  }

  if (check) {
    err = mysofa_check(hrtf);
    if (err != MYSOFA_OK) {
      fprintf(stderr, "Error checking file %s. Error code: %d:%s\n", filename,
              err, getErrorText(err));
      exit(EXIT_FAILURE);
    }
  }

  printJson(stdout, hrtf, sanitize);

  mysofa_free(hrtf);

  return 0;
}
