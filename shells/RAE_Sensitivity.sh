
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/RAE_Sensitivity/RAE_Sensitivity

splitter < split.inp 
cfl3dcmplx_seq < cfl3d.inp
cat ./cfl3d.out | grep    'timing for complete run - time in seconds'   -A8

splitter < split.inp 
cfl3d_seq < cfl3d.inp_+a 
cat ./cfl3d.out_+a | grep    'timing for complete run - time in seconds'   -A8
cfl3d_seq < cfl3d.inp_-a 
cat ./cfl3d.out_-a | grep    'timing for complete run - time in seconds'   -A8

Get_FD  <<EOF
restart.bin_+a
restart.bin_-a
1.e-6
Finite_Diff_1.e-6
0
EOF

cd ../../../../../shells