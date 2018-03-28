prep %1
if errorlevel == 1 goto done
profile %1 %2 %3 %4 %5 %6 %7 %8 %9
if errorlevel == 1 goto done
prep /M %1
if errorlevel == 1 goto done
plist %1 > %1.lst
del %1.pbi %1.pbo %1.pbt
:done
