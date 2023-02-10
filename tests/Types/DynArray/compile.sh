#!/bin/bash

if [[ ! -d "obj" ]]; then
	mkdir obj
fi
if [[ ! -d "bin" ]]; then
	mkdir bin
fi

export CPP=g++

$CPP -c -g -O2 -oobj/main.o perf/stdvec.cpp -I../../../include &&
$CPP -c -g -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include &&
$CPP -obin/stdvec obj/main.o obj/Malloc.o &&

$CPP -c -g -O2 -oobj/dynarr.o perf/dynarr.cpp -I../../../include &&
$CPP -c -g -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include &&
$CPP -obin/dynarr obj/dynarr.o obj/Malloc.o ||

read -rsp $'Press any key to continue...' -n 1 && echo
