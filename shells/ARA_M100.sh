cd ../cfl3d.larc.nasa.gov/Cfl3dv6/3DTestcases/ARA_M100/ARA_M100

splitter < ./split.inp_16blk 
amplxe-cl -collect hotspots mpirun -np 17 cfl3d_mpi < ./cfl3d.inp_16blk
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A24

cd ../../../../../shells