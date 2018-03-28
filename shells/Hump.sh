export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Hump/Humpcase
splitter < ./split.inp 
cfl3d_seq < ./hump2_e_oscillate_newtop.inp_1
cp cfl3d.out cfl3d.out1
cat cfl3d1.out | grep    'timing for complete run - time in seconds'   -A8

cfl3d_seq < ./hump2_e_oscillate_newtop.inp_2  
cp cfl3d.out cfl3d.out2
cat cfl3d2.out | grep    'timing for complete run - time in seconds'   -A8

cfl3d_seq < ./hump2_e_oscillate_newtop.inp_3 
cp cfl3d.out cfl3d.out3
cat cfl3d3.out | grep    'timing for complete run - time in seconds'   -A8
 
cd ../../../../../shells