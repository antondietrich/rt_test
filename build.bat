@echo off
setlocal

if /I "%1"=="--profile" set profileEnable=-D_PROFILE_
if /I "%1"=="/profile" set profileEnable=-D_PROFILE_
if /I "%2"=="--profile" set profileEnable=-D_PROFILE_
if /I "%2"=="/profile" set profileEnable=-D_PROFILE_

if /I "%1"=="" goto DEBUG
if /I "%1"=="--debug" goto DEBUG
if /I "%1"=="/debug" goto DEBUG

if /I "%1"=="--release" goto RELEASE
if /I "%1"=="/release" goto RELEASE
if /I "%1"=="/profile" goto RELEASE


:PROFILE
set profileEnable=-D_PROFILE_
goto RELEASE

:DEBUG
set compilerFlagsSpecific=-Od -Zi -MTd -D_DEBUG_=1 -DDEBUG=1
set output=-Fdbin\ -Fobuild\ -Febin\rttest.exe
echo Build config: Debug
goto COMMON

:RELEASE
set compilerFlagsSpecific=-Ox -MT -D_RELEASE_=1 -DNDEBUG
echo Build config: Release
goto COMMON

:COMMON
if not exist "build" md build
if not exist "bin" md bin
del bin\*.pdb > NUL 2> NUL

set compilerFlagsCommon=-MP -Oi -EHa- -GR- -W4 -nologo -D_CRT_SECURE_NO_WARNINGS %profileEnable% -DWITH_EDITOR=1
set suppressedWarnings=-wd4100 -wd4505 -wd4201 -wd4127

set linkerFlagsExe=/link /INCREMENTAL:NO
set sourceExe=src\main.cpp


set outputExe=-Fdbin\ -Fobuild\ -Febin\rttest.exe
goto BUILD

:BUILD
cl %compilerFlagsCommon% %compilerFlagsSpecific% %suppressedWarnings% %outputExe% %sourceExe% %linkerFlagsExe% user32.lib Gdi32.lib Winmm.lib
goto END

:NOCANDO
echo Unable to perform build
goto END

:END
endlocal
