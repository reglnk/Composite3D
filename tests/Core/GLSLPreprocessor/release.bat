@echo off

if not exist obj mkdir obj
if not exist bin mkdir bin

REM echo main.o             && g++ -c -O2 -I../../../include -oobj/main.o src/main.cpp || goto exit_err
REM echo Malloc.o           && g++ -c -O2 -I../../../include -oobj/Malloc.o ../../../source/Core/Malloc.cpp || goto exit_err
echo GLSLPreprocessor.o && g++ -c -O2 -I../../../include -oobj/GLSLPreprocessor.o ../../../source/Core/GLSLPreprocessor.cpp || goto exit_err
REM echo FileSystem.o       && g++ -c -O2 -I../../../include -oobj/FileSystem.o ../../../source/Core/FileSystem.cpp || goto exit_err
REM echo OSSpecFileSystem.o && g++ -c -O2 -I../../../include -oobj/OSSpecFileSystem.o ../../../source/Core/OSSpecific/FileSystem.cpp || goto exit_err

echo Linking... && g++ obj/main.o obj/Malloc.o obj/GLSLPreprocessor.o obj/FileSystem.o obj/OSSpecFileSystem.o -obin/preproc.exe || goto exit_err

goto exit_ok

:exit_err
pause

:exit_ok
