@echo off
setlocal

set system=WIN32

if "%1" == "" goto doit
if "%1" == "64" set system=WIN64
if "%1" == "-64" set system=WIN64
if "%1" == "64bit" set system=WIN64
if "%1" == "-64bit" set system=WIN64

:doit
if %system% == WIN64 (
  set args=-64
) else (
  set args=
)
if not exist make.%system% cmd /c configure.bat %args%
nmake %1

