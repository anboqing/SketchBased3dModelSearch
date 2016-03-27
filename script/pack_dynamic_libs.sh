#!/bin/sh

for file in `ldd ./SketchSearchDemo | awk '{print $3}' | sed '1d'`
do
    `cp $file ./libs`
done

exit 0
