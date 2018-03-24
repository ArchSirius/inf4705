#!/bin/bash

echo "algo,taille,temps" > ./results.csv

for algo in {"stdsort","qsort","insertion","merge","mergeSeuil"}; do
        for ex in $(ls testset_*); do
            size=$(echo $ex | cut -d_ -f2)
            t=$(./tp.sh -e ${ex} -a $algo -t)
            echo $algo,$size,$t
        done
done >> results.csv
