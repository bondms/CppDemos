#!/bin/bash

set -x

set -e
set -o pipefail
set -u

HERE="$(readlink --canonicalize-existing "$(dirname "$0")")"
[[ -d "${HERE}" ]] || exit 1

find "${HERE}/." \
    \( -name "*.cpp" -o -name "*.cc" -o -name "*.h" \) \
    -print0 | xargs --null clang-format -i -- || exit 1
