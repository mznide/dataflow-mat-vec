#!/bin/bash


EXE=$1

name=${EXE#../}
name=${name%%/*}
logfile="$name.txt"


test -f $logfile && exit

n=1024
while (( n <= 16384 )); do
	$EXE --size $n | tee -a $logfile
	(( n += 1024 ))
done

