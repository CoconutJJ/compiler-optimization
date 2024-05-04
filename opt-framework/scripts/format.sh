#!/bin/sh

for f in $(find $1 -name "*.c" -o -name "*.h" | grep -v build/)
do
    echo "clang-format -i ${f}"
    clang-format -i $f
done