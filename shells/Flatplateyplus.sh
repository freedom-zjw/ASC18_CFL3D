export PATH=~/zjw/CFL3D/build/bin:$PATH

cd ../cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Flatplateyplus/Flatplateyplus
splitter < ./splity+.02.inp 
cfl3d_seq < ./grid_y+.02sa.inp
cp cfl3d.out cfl3d_.02sa.out
cp cfl3d.res cfl3d_.02sa.res
cat  cfl3d_.02sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+.02sst.inp 
cp cfl3d.out cfl3d_.02sst.out
cp cfl3d.res cfl3d_.02sst.res
cat  cfl3d_.02sst.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./splity+.23.inp 
cfl3d_seq < ./grid_y+.23sa.inp
cp cfl3d.out cfl3d_.23sa.out
cp cfl3d.res cfl3d_.23sa.res
cat  cfl3d_.23sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+.23sst.inp 
cp cfl3d.out cfl3d_.23sst.out
cp cfl3d.res cfl3d_.23res.out
cat  cfl3d_.23sst.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./splity+.51.inp 
cfl3d_seq < ./grid_y+.51sa.inp
cp cfl3d.out cfl3d_.51sa.out
cp cfl3d.res cfl3d_.51sa.res
cat  cfl3d_.51sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+.51sst.inp 
cp cfl3d.out cfl3d_.51sst.out
cp cfl3d.res cfl3d_.51sst.res
cat  cfl3d_.51sst.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./splity+1.15.inp 
cfl3d_seq < ./grid_y+1.15sa.inp
cp cfl3d.out cfl3d_1.15sa.out
cp cfl3d.res cfl3d_1.15sa.res
cat  cfl3d_1.15sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+1.15sst.inp 
cp cfl3d.out cfl3d_1.15sst.out
cp cfl3d.res cfl3d_1.15sst.res
cat  cfl3d_1.15sst.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./splity+2.3.inp 
cfl3d_seq < ./grid_y+2.3sa.inp
cp cfl3d.out cfl3d_2.3sa.out
cp cfl3d.res cfl3d_2.3sa.res
cat  cfl3d_2.3sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+2.3sst.inp 
cp cfl3d.out cfl3d_2.3sst.out
cp cfl3d.res cfl3d_2.3sst.res
cat  cfl3d_2.3sst.out | grep    'timing for complete run - time in seconds'   -A8


splitter < ./splity+4.6.inp 
cfl3d_seq < ./grid_y+4.6sa.inp
cp cfl3d.out cfl3d_4.6sa.out
cp cfl3d.res cfl3d_4.6sa.res
cat  cfl3d_4.6sa.out | grep    'timing for complete run - time in seconds'   -A8


cfl3d_seq < ./grid_y+4.6sst.inp 
cp cfl3d.out cfl3d_4.6sst.out
cp cfl3d.res cfl3d_4.6sst.res
cat  cfl3d_4.6sst.out | grep    'timing for complete run - time in seconds'   -A8

cd ../../../../../shells