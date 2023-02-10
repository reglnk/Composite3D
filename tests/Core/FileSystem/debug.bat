@echo off
g++ -c -g -oobj/d_main.o src/main.cpp -I../../../include
g++ -c -g -oobj/d_Malloc.o ../../../source/Core/Malloc.cpp -I../../../include
g++ -c -g -oobj/d_FileSystem.o ../../../source/Core/FileSystem.cpp -I../../../include
g++ -c -g -oobj/d_OSspecFileSystem.o ../../../source/Core/OSSpecific/FileSystem.cpp -I../../../include
pause
g++ -obin/d_fs obj/d_main.o obj/d_Malloc.o obj/d_FileSystem.o obj/d_OSspecFileSystem.o
pause
