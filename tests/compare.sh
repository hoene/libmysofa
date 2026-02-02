#!/bin/sh

usage() {
    cat >/dev/stderr <<EOF
usage: $0 <sofafile> [<mysofa2json>]

  <sofafile>: SOFA file (without the .sofa extension) to test
  <mysofa2json>: 'mysofa2json' executable

the test expects a <sofafile>.json.bz2 reference file,
which is compared against the output of mysofa2json.

$*
EOF
    exit 1
}

sofafile="${1}.sofa"
referencefile="${1}.json.bz2"
mysofa2json="${2:-../build/src/mysofa2json}"

test -n "$1" || usage
test -e "${sofafile}" || usage "No such file: '${sofafile}'"
test -e "${referencefile}" || usage "No such file: '${referencefile}'"
test -x "${mysofa2json}" || usage "No mysofa2json executable: '${mysofa2json}'"

SCRIPTDIR=${0%/*}
NODEJS=$(command -v node nodejs false | head -1)
TMP1="$(mktemp -p . tmp1-XXXXXXXX.json)"
TMP2="$(mktemp -p . tmp2-XXXXXXXX.txt)"
TMP3="$(mktemp -p . tmp3-XXXXXXXX.json)"

"${mysofa2json}" -c -o "$TMP1" -s "${sofafile}" 2>"$TMP2"

ret=$?
if [ "$ret" != 0 ]; then
    cat "$TMP2"
    echo Error libmysofa $ret
    exit $ret
fi

cp -f "${SCRIPTDIR}/json-diff.js" . 2>/dev/null || true
bunzip2 -c -k "${referencefile}" >"$TMP3"
"${NODEJS}" ./json-diff.js "$TMP1" "$TMP3"
ret=$?
if [ "$ret" != 0 ]; then
    echo Diff $ret
    exit $ret
fi
echo ok
rm "$TMP1" "$TMP2" "$TMP3"
