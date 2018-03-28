
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Transdiff/Transdiff

splitter < ./split.inp_1blk 
cfl3d_seq < ./cfl3d.inp_ws 
cp cfl3d.out cfl3d_ws.out
cp cfl3d.res cfl3d_ws.res
cat  cfl3d_ws.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < cfl3d.inp_ss 
cp cfl3d.out cfl3d_ss.out
cp cfl3d.res cfl3d_ss.res
cat cfl3d_ss.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells