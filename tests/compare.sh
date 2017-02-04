#!/bin/bash -e

../build/src/mysofa2json "$1" >tmp1.json 2>tmp1.txt 

ret=$?
if [ "$ret" != 0 ]; then 
    cat tmp1.txt
    echo Error libmysofa $ret
    exit $ret
fi

cat "$1" | docker run -i sofa2json  /bin/bash -c 'cat >test.sofa && ./sofa2json test.sofa' >tmp2.json 2>tmp2.txt
ret=$?
if [ "$ret" != 0 ]; then 
    cat tmp2.txt
    echo Error libsofa $ret
    exit $ret
fi

cp -f ../tests/json-diff.js . 2>/dev/null || true
node ./json-diff.js ./tmp1.json ./tmp2.json
ret=$?
if [ "$ret" != 0 ]; then 
    echo Diff $ret
    exit $ret
fi
echo ok
rm tmp1.json tmp1.txt tmp2.json tmp2.txt

