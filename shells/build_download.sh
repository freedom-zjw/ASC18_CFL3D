#----------------------------------------------- bulid
cd ..
cd external
chmod 700 install_cgns
./install_cgns

cd ../build
make scruball
rm -rf bin
chmod 700 makeall.sh
./makeall.sh
cd ..

#----------------------------------------------- download test
wget -r -np -A gif,f,Z,gz -l 1 https://cfl3d.larc.nasa.gov/Cfl3dv6/cfl3dv6_testcases.html


cd cfl3d.larc.nasa.gov/Cfl3dv6/2DTestcases/Flatplate
gunzip Flatplate.tar.Z
tar -xvf Flatplate.tar
cd ..

cd Flatplateskew
gunzip Flatplateskew.tar.Z
tar -xvf Flatplateskew.tar
cd ..

cd Flatplateyplus
gunzip Flatplateyplus.tar.gz
tar -xvf Flatplateyplus.tar
cd ..

cd Backstep
gunzip Backstep.tar.gz
tar -xvf Backstep.tar
cd ..

cd Transdiff
gunzip Transdiff.tar.Z
tar -xvf Transdiff.tar
cd ..

cd RAE_Sensitivity
gunzip RAE_Sensitivity.tar.gz
tar -xvf RAE_Sensitivity.tar
cd ..

cd Ramp
gunzip Ramp.tar.Z
tar -xvf Ramp.tar
cd ..

cd Cylinder
gunzip Timeaccstudy.tar.Z
tar -xvf Timeaccstudy.tar
cd ..

cd  Ejectornozzle
gunzip Ejectornozzle.tar.gz
tar -xvf Ejectornozzle.tar
cd ..

cd Pitch
gunzip Pitch0012.tar.Z
tar -xvf Pitch0012.tar
cd ..

cd Rotorstator
gunzip Rotorstator.tar.Z
tar -xvf Rotorstator.tar
cd ..

cd Hump
gunzip Humpcase.tar.gz
tar -xvf Humpcase.tar
cd ..

cd Curvature
gunzip SoMellor.tar.gz
tar -xvf SoMellor.tar
cd ../..

cd 3DTestcases/Axibump
gunzip Axibump.tar.gz
tar -xvf Axibump.tar
cd ..

cd  ONERA_M6
gunzip ONERA_M6.tar.Z
tar -xvf ONERA_M6.tar
cd ..

cd ARA_M100
gunzip ARA_M100.tar.Z
tar -xvf ARA_M100.tar
cd ..

cd Delta
gunzip Delta_cgns.tar.Z
tar -xvf Delta_cgns.tar
cd ..
