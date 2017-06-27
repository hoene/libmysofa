# libmysofa

## Introduction

This is a simple set of C functions to read AES SOFA files, if they contain HRTFs
stored according to the AES69-2015 standard [http://www.aes.org/publications/standards/search.cfm?docID=99].

## Compile

Enter following commands

> cd build

> cmake -DCMAKE_BUILD_TYPE=Debug ..

> make all test

If you need an Debian package, call

> cd build && cpack

## Usage 

Libmysofa has a few main function calls.  

To read a SOFA file call 

```
#include <mysofa.h>

int filter_length;
int err;
struct MYSOFA_EASY *hrtf;

hrtf = mysofa_open("file.sofa", 48000, &filter_length, &err);
if(hrtf==NULL) 
	return err;
```

To free the HRTF structure, call:
```
mysofa_close(hrtf);
```

If you need HRTF filter for a given coordinate, just call
```
short leftIR[filter_length];
short rightIR[filter_length];
int leftDelay;          // unit is samples
int rightDelay;         // unit is samples

mysofa_getfilter_short(hrtf, x, y, z, leftIR, rightIR, &leftDelay, &rightDelay);
```
and then delay the audio signal by leftDelay and rightDelay samples and do a FIR filtering with leftIR and rightIR. Alternative, if you are using float values for the filtering, call
```
float leftIR[filter_length]; // [-1. till 1]
float rightIR[filter_length];
float leftDelay;          // unit is sec.
float rightDelay;         // unit is sec.

mysofa_getfilter_float(hrtf, x, y, z, leftIR, rightIR, &leftDelay, &rightDelay);
```

If you have spherical coordinates but you need Cartesian coordinates, call
```
void mysofa_s2c(float values[3])
```
which phi (azimuth measure counterwise from the X axis), theta (elevation measured up from the x-y plane), and r (distance between listener and source) as parameters in the float array and x,y,z as response in the same array. Similar, call
```
void mysofa_c2s(float values[3])
```
The coordinate system is defined in the SOFA specification and is the same as in the SOFA file. Typically, the x axis vector (1 0 0) is the listening direction. The y axis (0 1 0) is the left side of the listener and z (0 0 1) is upwards.


Sometimes, you want to use multiple SOFA filters or if you have to open a SOFA file multiple times, you may use
```
hrtf1 = mysofa_open_cached("file.sofa", 48000, &filter_length, &err);
hrtf2 = mysofa_open_cached("file.sofa", 48000, &filter_length, &err);
hrtf3 = mysofa_open_cached("file.sofa", 8000, &filter_length, &err);
hrtf3 = mysofa_open_cached("file2.sofa", 8000, &filter_length, &err);
mysofa_close_cached(hrtf1);
mysofa_close_cached(hrtf2);
mysofa_close_cached(hrtf3);
mysofa_close_cached(hrtf4);
...
mysofa_cache_release_all();
```
Then, all HRTFs having the same filename and sampling rate are stored only once in memory. 

If your program is using several threads, you must use appropriate synchronisation mechanisms so only a single thread can access the library at a given time.

## Disclaimer

The SOFA files are from https://www.sofaconventions.org/, Piotr Majdak <piotr@majdak.com>. The K-D tree algorithm is by John Tsiombikas <nuclear@member.fsf.org>. The resampler is by Jean-Marc Valin. The remaining source code is by Christian Hoene <christian.hoene@symonics.com>, Symonics GmbH, and available under BSD-3-Clause license. This work has been funded by German Federal Ministry of Education and Research, funding code 01IS14027A.


