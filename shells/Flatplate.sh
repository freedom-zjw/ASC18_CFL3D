export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Flatplate/Flatplate
splitter < ./split.inp_1blk 
cfl3d_seq < ./grdflat5.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells