#!/bin/bash


EXE=$1

name=${EXE#../}
name=${name%%/*}
logfile="$name.txt"


test -f $logfile && exit

m=0

while (( m <= 5)); do
	n=1024
	while (( n <= 16384 )); do
		sleep 2
		$EXE --size $n | tee -a $logfile
		(( n += 1024 ))
	done

	(( m += 1 ))
done

