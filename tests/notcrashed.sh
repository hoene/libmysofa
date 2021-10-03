#!/bin/sh
test -f "$1".sofa || exit 128
../build/src/mysofa2json "$1".sofa >/dev/null 2>/dev/null 
ret=$?
if [ "$ret" -ge 128 ]; then
	    echo mysofa2json crashed with $ret opening$1.sofa
            exit $ret
fi
echo good
exit 0

