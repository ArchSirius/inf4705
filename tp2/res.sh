#!/bin/bash

EXEC=./tp.sh
OUTPUT=results.csv

echo "algo,taille,temps" > $OUTPUT

#vorace
algo=vorace
for n in {100,500,1000,5000,10000,50000,100000}; do
	for i in {1..10}; do
		ex=b_${n}_${i}.txt
		sum=0
		for j in {0..9}; do
			t=$($EXEC -e $ex -a $algo -t)
			sum=$(echo $sum + $t | bc -l)
		done
		value=$(echo $sum / 10 | bc -l)
		# Taken from https://stackoverflow.com/a/229606
		if [[ $LANG = *"fr"* ]]; then
			value=$(echo $value | tr . ,)
			# Taken from https://unix.stackexchange.com/a/175746
			t=$(printf "%.6f\n" $value | tr , .)
			echo $algo,$n,$t
		else
			t=$(printf "%.6f\n" $value)
			echo $algo,$n,$t
		fi
	done
done >> $OUTPUT

#progdyn
algo=progdyn
for n in {100,500,1000,5000,10000}; do
	for i in {1..10}; do
		ex=b_${n}_${i}.txt
		t=$($EXEC -e $ex -a $algo -t)
		echo $algo,$n,$t
	done
done >> $OUTPUT

#tabou
algo=tabou
for n in {100,500,1000,5000,10000}; do
	for i in {1..10}; do
		ex=b_${n}_${i}.txt
		t=$($EXEC -e $ex -a $algo -t)
		echo $algo,$n,$t
	done
done >> $OUTPUT

#lents
echo progdyn,50000,$($EXEC -e b_50000_1.txt -a progdyn -t) >> $OUTPUT
echo tabou,50000,$($EXEC -e b_50000_1.txt -a tabou -t) >> $OUTPUT
echo progdyn,100000,$($EXEC -e b_100000_1.txt -a progdyn -t) >> $OUTPUT
echo tabou,100000,$($EXEC -e b_100000_1.txt -a tabou -t) >> $OUTPUT
