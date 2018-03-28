
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/3DTestcases/Axibump/Axibump

splitter < ./split.inp_1blk 
cfl3d_seq < ./bumpperiodic.inp 
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

splitter < ./split.inp_3blk 
mpirun -np 4 cfl3d_mpi < ./bumpperiodic.inp_3blk
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A10
 
cd ../../../../../shells