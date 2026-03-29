#!/bin/sh
usage() {
    cat >/dev/stderr <<EOF
usage: $0 <sofafile> [<mysofa2json>]

  <sofafile>: SOFA file (without the .sofa extension) to test
  <mysofa2json>: 'mysofa2json' executable

tests whether <sofafile>.sofa can be loaded without crashing.

$*
EOF
    exit 1
}

sofafile="${1}.sofa"
mysofa2json="${2:-../build/src/mysofa2json}"

#test -e "${sofafile}" || usage "No such file: '${sofafile}'"
test -n "${1}" || usage
test -f "${sofafile}" || exit 128
test -x "${mysofa2json}" || usage "No mysofa2json executable: '${mysofa2json}'"

"${mysofa2json}" -c "${sofafile}" >/dev/null 2>/dev/null
ret=$?
if [ "$ret" -ge 128 ]; then
    echo mysofa2json crashed with $ret opening '${sofafile}'
    exit $ret
fi
echo good
exit 0
