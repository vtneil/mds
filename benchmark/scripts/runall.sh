#!/bin/bash
# runall.sh

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <docker-image-file> <bind-1> <bind-2> <args...?>"
    exit 1
fi

image_file="$1"
shift

bind1="$1"
shift

bind2="$1"
shift

args=""
for arg in "$@"; do
    args+="'$arg' "
done
