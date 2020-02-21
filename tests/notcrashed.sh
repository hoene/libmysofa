#!/bin/sh

../build/src/mysofa2json "$1".sofa >/dev/null 2>/dev/null 
ret=$?
if [ "$ret" -ge 128 ]; then
   if [ "$ret" -ge 128 ]; then
	    echo mysofa2json crashed with $ret opening$1.sofa
            exit $ret
   fi
fi
echo good
exit 0

