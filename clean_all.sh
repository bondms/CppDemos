#!/bin/bash

set -x

set -e
set -o pipefail
set -u

HERE="$(readlink -f "$(dirname "$0")")"
[[ -d "${HERE}" ]] || exit $?

find "${HERE}" -maxdepth 2 -mindepth 2 -name Makefile -type f -print0 | bash -c "
  while read -r -d $'\0' F
  do
    echo Processing Makefile \"\$F\"
    DIR=\$(dirname \"\$F\")
    pushd \"\${DIR}\" > /dev/null || exit \$?
    pwd
    make clean || exit \$?
    popd > /dev/null || exit \$?
  done" || exit $?

find "${HERE}" -maxdepth 2 -mindepth 2 -name WORKSPACE -type f -print0 | bash -c "
  while read -r -d $'\0' F
  do
    echo Processing WORKSPACE \"\$F\"
    DIR=\$(dirname \"\$F\")
    pushd \"\${DIR}\" > /dev/null || exit \$?
    pwd
    bazel clean --expunge || exit \$?
    popd > /dev/null || exit \$?
  done" || exit $?

if [[ -e "${HERE}/thirdparty" ]]
then
  find "${HERE}/thirdparty" -delete || exit $?
fi
