#!/bin/bash

function print_settings {
#	egrep -r "int nMax = " *
	egrep -r "int vectorSize = " *
}

function prepare_vec {
	vec=$1
	for p in Basic Transposed Tiled; do
		src=${p}Vec
		dst=${p}Vec$1
		cp -r $src $dst
		f=$(egrep -r "vectorSize = " $dst | cut -d: -f1)
		{
			rm "$f"
			sed -E -e "s/vectorSize = [[:digit:]]+/vectorSize = $vec/" >"$f"
		} <"$f"	
	done
}


cd ..

print_settings
echo

prepare_vec 2
prepare_vec 4
prepare_vec 8
prepare_vec 16
prepare_vec 32

print_settings

exit 0
