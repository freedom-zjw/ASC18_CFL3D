#make all
make scruball  -j
make linkall -j
make cfl3d_libs -j
make cfl3d_mpi  -j
make cfl3d_seq  -j   
make cfl3dcmplx_libs  -j
make cfl3dcmplx_mpi  -j
make cfl3dcmplx_seq  -j
make precfl3d    -j    
make ronnie  -j    
make preronnie  -j   
make maggie -j 
make splitter -j       
make splittercmplx -j
make cfl3d_tools -j 


mkdir bin
cp cfl/seq/cfl3d_seq  bin/
cp cfl/mpi/cfl3d_mpi  bin/
cp cflcmplx/mpi/cfl3dcmplx_mpi  bin/
cp cflcmplx/seq/cfl3dcmplx_seq  bin/
cp ron/seq/ronnie  bin/
cp preron/seq/preronnie  bin/
cp mag/seq/maggie bin/
cp split/seq/splitter bin/
cp splitcmplx/seq/splittercmplx bin/
cp tools/seq/grid_perturb_cmplx   bin/                      
cp tools/seq/grid_perturb  bin/                       
cp tools/seq/Get_FD bin/                         
cp tools/seq/moovmaker bin/                        
cp tools/seq/plot3dg_to_cgns  bin/                       
cp tools/seq/XINTOUT_to_ovrlp   bin/                      
cp tools/seq/p3d_to_INGRID  bin/                       
cp tools/seq/INGRID_to_p3d  bin/                       
cp tools/seq/cfl3d_to_pegbc bin/                        
cp tools/seq/everyother_xyz  bin/                      
cp tools/seq/initialize_field bin/                       
cp tools/seq/p3d_to_cfl3drst  bin/                      
cp tools/seq/v6_restart_mod bin/                       
cp tools/seq/v6inpdoubhalf  bin/                      
cp tools/seq/cgns_to_cfl3dinput  bin/                      
cp tools/seq/cgns_readhist      bin/                  
cp tools/seq/v6_ronnie_mod   bin/                     
cp tools/seq/cfl3d_to_nmf  bin/                      
cp tools/seq/cfl3dinp_to_FVBND  bin/                      
cp tools/seq/nmf_to_cfl3dinput   bin/                   
cp tools/seq/gridswitchijk  bin/              
cp tools/seq/v6inpswitchijk bin/
