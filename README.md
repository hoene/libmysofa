<div align="right">
<a href="https://symonics.com/">
	<img alt="Symonics MySofa" width="320px" src="https://raw.githubusercontent.com/hoene/libmysofa/master/symonics-mysofa.png"/>
</a>
</div

#

# libmysofa

## Introduction
This is a simple set of C functions to read AES SOFA files, if they contain HRTFs
stored according to the AES69-2015 standard [http://www.aes.org/publications/standards/search.cfm?docID=99].

## Badges

<div align="center">
<a href="https://travis-ci.org/hoene/libmysofa">
	<img alt="Travis CI Status" src="https://travis-ci.org/hoene/libmysofa.svg?branch=master"/>
</a>

<a href="https://scan.coverity.com/projects/hoene-libmysofa">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/13030/badge.svg"/>
</a>

<a href="https://ci.appveyor.com/project/hoene/libmysofa-s142k">
<img alt="AppVeyor Status" src="https://ci.appveyor.com/api/projects/status/mk86lx4ux2jn9tpo/branch/master?svg=true"/>
</a>

<a href="ttps://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=GUN8R6NUQCS3C" <img alt="Donate" src="https://symonics.com/Donate-PayPal-green.svg"/></a>

</div>

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
struct MYSOFA_EASY *hrtf = NULL;

hrtf = mysofa_open("file.sofa", 48000, &filter_length, &err);
if(hrtf==NULL)
	return err;
```

This call will normalize your hrtf data upon opening. For non-normalized data, replace the call to mysofa_open by:

```
hrtf = mysofa_open_no_norm("file.sofa", 48000, &filter_length, &err);
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
with phi (azimuth in degree, measured counterclockwise from the X axis), theta (elevation in degree,  measured up from the X-Y plane), and r (distance between listener and source) as parameters in the float array and x,y,z as response in the same array. Similar, call
```
void mysofa_c2s(float values[3])
```
The coordinate system is defined in the SOFA specification and is the same as in the SOFA file. Typically, the X axis vector (1 0 0) is the listening direction. The Y axis (0 1 0) is the left side of the listener and Z (0 0 1) is upwards.


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

## OS support

Libmysofa compiles for Linux operating systems, OSX and Windows. By default, each commit is compiled with Travis CI under Ubuntu 14.04 and OSX 7.3 and with AppVeyor for Windows Visual Studio 2015 on a x64 system. In addition, FFmpeg is compiling libmysofa with MinGW under Windows using their own build system.


## References

 * Christian Hoene and Piotr Majdak, "HDF5 under the SOFA – A 3D audio case in HDF5 on embedded and mobile devices", HDF Blog, https://www.hdfgroup.org/2017/04/hdf5-under-the-sofa-hdf5-on-embedded-and-mobile-devices/, April 26, 2017.
 * Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi, "MySofa: Design Your Personal HRTF", Audio Engineering Society
 Convention Paper 9764, Presented at the 142nd Convention, May 2017, Berlin, Germany, http://www.aes.org/e-lib/browse.cfm?elib=18640

## Disclaimer

The SOFA files are from https://www.sofaconventions.org/, Piotr Majdak <piotr@majdak.com>. The K-D tree algorithm is by John Tsiombikas <nuclear@member.fsf.org>. The resampler is by Jean-Marc Valin. The remaining source code is by Christian Hoene <christian.hoene@symonics.com>, <a href="https://symonics.com/">Symonics GmbH</a>, and available under BSD-3-Clause license. This work has been funded by German <a href="https://www.bmbf.de">Federal Ministry of Education and Research</a>, funding code 01IS14027A.

## Donate

Support the development and maintained of libmysofa by donating:

<div align="center">
<form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
<input type="hidden" name="cmd" value="_donations">
<input type="hidden" name="business" value="AB4ATNFQKCZN6">
<input type="hidden" name="lc" value="DE">
<input type="hidden" name="item_name" value="Symonics GmbH">
<input type="hidden" name="item_number" value="libmysofa">
<input type="hidden" name="no_note" value="0">
<input type="hidden" name="cn" value="Message to Christian Hoene">
<input type="hidden" name="no_shipping" value="2">
<input type="hidden" name="currency_code" value="EUR">
<input type="hidden" name="bn" value="PP-DonationsBF:btn_donateCC_LG.gif:NonHosted">
<input type="image" src="https://www.paypalobjects.com/en_US/DE/i/btn/btn_donateCC_LG.gif" border="0" name="submit" alt="PayPal - The safer, easier way to pay online!">
<img alt="" border="0" src="https://www.paypalobjects.com/de_DE/i/scr/pixel.gif" width="1" height="1">
</form>
</div>
