@echo off

if not defined CPP (
	set CPP=g++
)

if not exist obj mkdir obj
if not exist bin mkdir bin

%CPP% -c -g -O2 -oobj/stdvec.o perf/stdvec.cpp -I../../../include || goto exit_err
%CPP% -c -g -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include || goto exit_err
%CPP% -obin/perf_stdvec.exe obj/stdvec.o obj/Malloc.o || goto exit_err

%CPP% -c -g -O2 -oobj/dynarr.o perf/dynarr.cpp -I../../../include || goto exit_err
%CPP% -c -g -O2 -oobj/Malloc.o ../../../source/Core/Malloc.cpp -I../../../include || goto exit_err
%CPP% -obin/perf_dynarr.exe obj/dynarr.o obj/Malloc.o || goto exit_err

goto exit_ok

:exit_err
pause

:exit_ok
