

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/3DTestcases/Delta/Delta_cgns

plot3dg_to_cgns < ./plot3dg_to_cgns.inp
cfl3d_seq < ./delta_cgns.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells