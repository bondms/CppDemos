#!/bin/bash

set -x

set -e
set -o pipefail
set -u

HERE="$(readlink -f "$(dirname "$0")")"
[[ -d "${HERE}" ]] || exit $?

find "${HERE}" -maxdepth 2 -mindepth 2 -name WORKSPACE -type f -print0 | bash -c "
    while read -r -d $'\0' F
    do
      echo Processing WORKSPACE \"\$F\"
      DIR=\$(dirname \"\$F\")
      pushd \"\${DIR}\" > /dev/null || exit \$?
      pwd
      bazel test ... || exit \$?
      popd > /dev/null || exit \$?
    done" || exit $?
