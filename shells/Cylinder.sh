

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Cylinder/Timeaccstudy
splitter < ./split.inp 
cfl3d_seq < ./cyl_start_1.inp
cat ./cfl3d.out_start_1 | grep    'timing for complete run - time in seconds'   -A8
cfl3d_seq < ./cyl_start_2.inp 
cat ./cfl3d.out_start_2 | grep    'timing for complete run - time in seconds'   -A8
cfl3d_seq < ./cyl_start_3.inp 
cat ./cfl3d.out_start_3 | grep    'timing for complete run - time in seconds'   -A8
cp restart.bin restart.binIC 

cp restart.binIC restart.bin
cfl3d_seq < ./cyl_0.2.inp
cat ./cfl3d.out_0.2 | grep    'timing for complete run - time in seconds'   -A8

cp restart.binIC restart.bin
cfl3d_seq < ./cyl_0.1.inp
cat ./cfl3d.out_0.1 | grep    'timing for complete run - time in seconds'   -A8

cp restart.binIC restart.bin
cfl3d_seq < ./cyl_0.05.inp
cat ./cfl3d.out_0.05 | grep    'timing for complete run - time in seconds'   -A8

cp restart.binIC restart.bin
cfl3d_seq < ./cyl_0.025.inp
cat ./cfl3d.out_0.025 | grep    'timing for complete run - time in seconds'   -A8

cp restart.binIC restart.bin
cfl3d_seq < ./cyl_0.0125.inp
cat ./cfl3d.out_0.0125 | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells