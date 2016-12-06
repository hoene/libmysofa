#!/bin/bash

../build/src/mysofa2json "$1" >tmp1.json 2>tmp1.txt 

ret=$?
if [ "$ret" != 0 ]; then 
    cat tmp1.txt
    echo Error libmysofa $ret
    exit $ret
fi

../tests/sofa2json "$1" >tmp2.json 2>tmp2.txt
ret=$?
if [ "$ret" != 0 ]; then 
    cat tmp2.txt
    echo Error libsofa $ret
    exit $ret
fi

cp -f ../tests/json-diff.js .
node ./json-diff.js ./tmp1.json ./tmp2.json
ret=$?
if [ "$ret" != 0 ]; then 
    echo Diff $ret
    exit $ret
fi

rm tmp1.json tmp1.txt tmp2.json tmp2.txt
