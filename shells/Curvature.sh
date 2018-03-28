export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Curvature/SoMellors

splitter < ./split_form_to_unform.inp 
cfl3d_seq < ./somellor_sa.inp 
cp cfl3d.out_sa cfl3d_sa.out
cp cfl3d.res_sa cfl3d_sa.res
cat  cfl3d_sa.out | grep    'timing for complete run - time in seconds'   -A8

cfl3d_seq < ./somellor_sarc.inp
cp cfl3d.out_sa cfl3d_sarc.out
cp cfl3d.out_res cfl3d_sarc.res
cat  cfl3d_sarc.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells