@echo off
setlocal

set zones=1 5 10 50 100 500 1000 5000 10000 50000 100000
set values=1 10 100 1000 10000 100000
rem for %%z in ( %zones% ) do (
rem for %%v in ( %values% ) do dbtest %%z %%v )
for %%z in ( %zones% ) do dbtest %%z 1
endlocal
