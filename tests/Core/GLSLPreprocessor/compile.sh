#!/bin/bash

if [[ ! -d "obj" ]]; then
	mkdir obj
fi
if [[ ! -d "bin" ]]; then
	mkdir bin
fi

export CPP=g++

$CPP -c -O2 -oobj/main.o src/main.cpp -I../../../include &&
$CPP -c -O2 -oobj/LogManager.o ../../../source/Core/LogManager.cpp -I../../../include &&
$CPP -c -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include &&
$CPP obj/* -obin/logmgr ||

read -rsp $'Press any key to continue...' -n 1 && echo
