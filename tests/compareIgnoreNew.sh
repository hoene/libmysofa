#!/bin/sh
mysofa2json=$2

SCRIPTDIR=${0%/*}
NODEJS=$(command -v node nodejs false | head -1)
TMP1=`mktemp -p . tmp1-XXXXXXXX.json`
TMP2=`mktemp -p . tmp2-XXXXXXXX.txt`
TMP3=`mktemp -p . tmp3-XXXXXXXX.json`

"$mysofa2json" -c -s -o "$TMP1" "$1".sofa 2>"$TMP2"

ret=$?
if [ "$ret" != 0 ]; then 
    cat "$TMP2"
    echo Error libmysofa $ret
    exit $ret
fi

cp -f "${SCRIPTDIR}/json-diffIgnoreNew.js" . 2>/dev/null || true
bunzip2 -c -k "$1".json.bz2 >"$TMP3"
"${NODEJS}" ./json-diffIgnoreNew.js "$TMP1" "$TMP3"
ret=$?
if [ "$ret" != 0 ]; then 
    echo Diff $ret
    exit $ret
fi
echo ok
rm "$TMP1" "$TMP2" "$TMP3"

