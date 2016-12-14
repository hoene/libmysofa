# libmysofa

## Introduction

This is a simple set of C functions to read AES SOFA files, if they contain HRTFs
stored according to the AES69-2015 standard [http://www.aes.org/publications/standards/search.cfm?docID=99].

Instead of the libhdf and libnetcdf, we are implementing a simple HDF5 reader following the specification [https://support.hdfgroup.org/HDF5/doc/H5.format.html].

For example, a SOFA to JSON converter written with NETCDF C++ has a size of 798387 bytes on Linux 64bit; written with libmysofa, it has a size of 90625.

## Compile

Enter following commands

> cd build

> cmake -DCMAKE_BUILD_TYPE=Debug ..

> make all test

and see how some tests fail. But do not worry, the issues are only minor.
    
## Usage 

A SOFA file can then be read with simple statements.

To read a SOFA file call 

```
#include <mysofa.h>

struct HRTF *hrtf = mysofa_load("file.sofa", &err);
```

To free the HRTF structure, call:
```
mysofa_free(hrtf);
```

## Disclaimer

The SOFA files are from https://www.sofaconventions.org/, Piotr Majdak <piotr@majdak.com>.
The source code is by Christian Hoene <christian.hoene@symonics.com>, Symonics GmbH, and available under Apache 2.0 license.
This work has been funded by Germman Federal Ministry of Education and Research, funding code 01IS14027A.


