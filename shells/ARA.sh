
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/3DTestcases/ARA_M100/ARA_M100
#splitter < ./split.inp_1blk 
#cfl3d_seq < ./cfl3d.inp_1blk 
#cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

splitter < ./split.inp_16blk 
mpirun -np 17 cfl3d_mpi < ./cfl3d.inp_16blk
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A24

cd ../../../../../shells
