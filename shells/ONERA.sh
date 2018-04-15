
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/3DTestcases/ONERA_M6/ONERA_M6
splitter < ./split.inp_1blk 
cfl3d_seq < ./cfl3d.inp_1blk
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

splitter < split.inp_32blk 
mpirun -np 9 cfl3d_mpi < cfl3d.inp_32blk
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A16

cd ../../../../../shells