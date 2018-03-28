export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Ejectornozzle/Ejectornozzle
splitter < ./split.inp 
cfl3d_seq < ./nozzle.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8
cd  ../../../../../shells