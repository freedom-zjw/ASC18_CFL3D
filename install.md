如果只要运行此处就不需要看了

# CFL3D 安装

~~~shell
git clone https://github.com/nasa/CFL3D.git
cd CFL3D
~~~

### 安装cgns

~~~shell
mkdir external
cd external
wget "https://github.com/CGNS/CGNS_archives/raw/master/cgnslib_2.5-5.tar.gz" -O cgnslib_2.5-5.tar.gz
tar xf cgnslib_2.5-5.tar.gz
cd cgnslib_2.5
./configure --prefix=$PWD/../cgns
make VERBOSE=1
mkdir -p ../cgns/lib
mkdir -p ../cgns/include
make install
~~~

## 安装CFL3D

~~~shell
cd ../../build/
./Install -single -fastio -linux_compiler_flags=Intel  -cgnsdir=dir
~~~

将dir修改为cgns安装路径中lib的路径

请查看 makefile中的LLIBS、LLIBS_SEQ、CGNS_INCDIR 的路径是否正确，可自行对makefile进行修改，前两个应为/lib,后者为/include

以下为make脚本在build中 直接使用makeall.sh可运行

~~~shell
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
~~~

清理make

~~~shell
make scruball
rm -rf bin
~~~

#CFL3D运行

## 下载测试例子

~~~shell
wget -r -np -A gif,f,Z,gz -l 1 https://cfl3d.larc.nasa.gov/Cfl3dv6/cfl3dv6_testcases.html
~~~

下载完成后算例位于

~~~
/cfl3d.larc.nasa.gov/Cfl3dv6/ 
~~~

具体运行方式请看[Test Cases](https://cfl3d.larc.nasa.gov/Cfl3dv6/cfl3dv6_testcases.html)

## 算例运行

结果文件为.out 和.res,请自行比对官网[Test Cases](https://cfl3d.larc.nasa.gov/Cfl3dv6/cfl3dv6_testcases.html)

以下为官网样例测试情况

~~~shell
Flatplate      #可用   
Flatplateskew  #可用   4个子算例
Flatplateyplus #可用   12个子算例
Backstep       #可用
Transdiff      #可用   2个子算例	
NACA 4412      #不可用 缺少PEGSUS
RAE_Sensitivity#可用   2个子算例
Ramp           #可用   2个子算例
Cylinder       #可用   8个子算例
N0012          #不可用 疑似输入数据有误
Ejectornozzle  #可用
Pitch          #可用   3个子算例
Rotorstator    #可用   
Hump           #可用   3个子算例
Curvature      #可用   2个子算例

#---------3D算例---------
Axibump        #可用   先跑单进程后跑多进程
ONERA          #可用   同上
ARA M100       #可用   同上  ARA_M100.sh 附带收集热点
ARA_M100_XMERA #不可用 缺少PEGSUS
Delta_cgns     #可用   
~~~





