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

cd precfl/seq 
#    mpif90 -DP3D_SINGLE -DLINUX -DINTEL -DPG -DCGNS -I../../../external/cgns/include -Wl,-Bdynamic -Wl,-z muldefs  -o precfl3d sizer.o get_cmd_args.o rpatch0.o outbuf.o getibk0.o global.o global2.o pointers.o compg2n.o umalloc.o cntsurf.o lead.o pre_patch.o pre_blockbc.o pre_embed.o pre_period.o usrint.o termn8.o getdhdr.o global0.o readkey.o parser.o ccomplex.o cgnstools.o setseg.o my_flush.o main.o  -L../../../external/cgns/lib  -lcgns  -L../../cfl/libs -lcommon

    mpiifort -DP3D_SINGLE -DLINUX -DINTEL -DCGNS  -I../../../external/cgns/include -z muldefs -xHost -traceback -fpe0 -o precfl3d sizer.o get_cmd_args.o rpatch0.o outbuf.o getibk0.o global.o global2.o pointers.o compg2n.o umalloc.o cntsurf.o lead.o pre_patch.o pre_blockbc.o pre_embed.o pre_period.o usrint.o termn8.o getdhdr.o global0.o readkey.o parser.o ccomplex.o cgnstools.o setseg.o my_flush.o main.o  -L../../../external/cgns/lib  -lcgns  -L../../cfl/libs -lcommon
cd ../..


mkdir bin
cd bin
## basic
ln -s ../cfl/seq/cfl3d_seq
## parallel version
ln -s ../cfl/mpi/cfl3d_mpi
## mesh block splitter
ln -s ../split/seq/splitter
## memory usage estimator
ln -s ../precfl/seq/precfl3d
## overset mesh related tool
ln -s ../mag/seq/maggie
## mesh deformation tool and its memory usage estimator
ln -s ../ron/seq/ronnie
ln -s ../preron/seq/preronnie
## complex version of cfl3d which is used to compulate flight derivative
ln -s ../cflcmplx/seq/cfl3dcmplx_seq
ln -s ../cflcmplx/mpi/cfl3dcmplx_mpi
ln -s ../splitcmplx/seq/splittercmplx
## cfl3d_tools
ln -s ../tools/seq/v6inpdoubhalf
ln -s ../tools/seq/Get_FD
ln -s ../tools/seq/initialize_field
ln -s ../tools/seq/nmf_to_cfl3dinput
ln -s ../tools/seq/cgns_to_cfl3dinput
ln -s ../tools/seq/v6inpswitchijk
ln -s ../tools/seq/v6_ronnie_mod
ln -s ../tools/seq/moovmaker
ln -s ../tools/seq/grid_perturb_cmplx
ln -s ../tools/seq/v6_restart_mod
ln -s ../tools/seq/gridswitchijk
ln -s ../tools/seq/cfl3d_to_nmf
ln -s ../tools/seq/cgns_readhist
ln -s ../tools/seq/p3d_to_cfl3drst
ln -s ../tools/seq/everyother_xyz
ln -s ../tools/seq/p3d_to_INGRID
ln -s ../tools/seq/v6_ronnie_mod.F90
ln -s ../tools/seq/INGRID_to_p3d
ln -s ../tools/seq/grid_perturb
ln -s ../tools/seq/cfl3dinp_to_FVBND
ln -s ../tools/seq/cfl3d_to_pegbc
ln -s ../tools/seq/plot3dg_to_cgns
ln -s ../tools/seq/XINTOUT_to_ovrlp

cd ..
