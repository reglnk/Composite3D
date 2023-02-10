@echo off
g++ -c -O2 -oobj/main.o src/main.cpp -I../../../include
g++ -c -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include
g++ -c -O2 -oobj/FileSystem.o ../../../source/Core/FileSystem.cpp -I../../../include
g++ -c -O2 -oobj/OSspecFileSystem.o ../../../source/Core/OSSpecific/FileSystem.cpp -I../../../include
pause
g++ -obin/fs obj/main.o obj/Malloc.o obj/FileSystem.o obj/OSspecFileSystem.o
pause
