#!/bin/sh
# for file in tests/*... basename
for f in ./*.py
do
    python $f
done
for f in ./tests/*.py
do
    python $f
done
