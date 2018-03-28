export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Backstep/Backstep

splitter < ./split.inp_1blk 
ronnie < ./ron_step_grdgen.inp
cfl3d_seq < ./step_grdgen.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells