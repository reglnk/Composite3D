@echo off

if not exist obj mkdir obj
if not exist bin mkdir bin

g++ -c -g -O2 -oobj/main.o src/main.cpp -I../../../include || goto exit_err
g++ -c -g -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include || goto exit_err

g++ obj/main.o obj/Malloc.o -obin/queue.exe || goto exit_err

goto exit_ok

:exit_err
pause

:exit_ok
