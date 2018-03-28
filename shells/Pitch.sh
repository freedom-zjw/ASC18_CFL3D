export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Pitch/Pitch0012
splitter < ./split.inp 
cfl3d_seq < ./n0012_ss.inp 
cp cfl3d.out cfl3d.out_ss
cat ./cfl3d.out_ss | grep    'timing for complete run - time in seconds'   -A8

cfl3d_seq < ./n0012_pitch.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8
 
cfl3d_seq < ./n0012_pitch2.inp 
cat ./cfl3d2.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells