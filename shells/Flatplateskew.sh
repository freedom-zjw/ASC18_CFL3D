
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Flatplateskew/Flatplateskew
splitter < ./split.inpnoskew 
cfl3d_seq < ./grdflat5_noskew.inp
cp cfl3d.out cfl3d_noskew.out
cp  cfl3d.res  cfl3d_noskew.res
cat  cfl3d_noskew.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./split.inp15 
cfl3d_seq < ./grdflat5_15.inp
cp cfl3d.out cfl3d_15.out
cp  cfl3d.res  cfl3d_15.res
cat  cfl3d_15.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./split.inp30 
cfl3d_seq < ./grdflat5_30.inp
cp cfl3d.out cfl3d_30.out
cp  cfl3d.res  cfl3d_30.res
cat  cfl3d_30.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./split.inp45 
cfl3d_seq < ./grdflat5_45.inp
cp cfl3d.out cfl3d_45.out
cp  cfl3d.res  cfl3d_45.res
cat  cfl3d_45.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells
