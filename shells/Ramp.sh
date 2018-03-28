export PATH=~/zjw/CFL3D/build/bin:$PATH
cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Ramp/Ramp

splitter < ./split_noembed.inp 
cfl3d_seq < ./ramp_noembed.inp 
cat ./cfl3d.out_noembed | grep    'timing for complete run - time in seconds'   -A8

splitter < ./split_embed.inp 
cfl3d_seq < ./ramp_embed.inp
cat ./cfl3d.out_embed | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells