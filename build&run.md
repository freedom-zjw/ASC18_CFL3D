# 运行方式

~~~shell
cd shells
chmod 700 *.sh
./build_download.sh #自动安装，下载测试
~~~

需要修改makefile的路径

~~~shell
vi makefile
#以下5处路径修改为安装路径
FFLAG        = -O2  -g -ffast-math -w -ip -fno-alias -xHost -traceback -fpe0 -module /home/asc18/zjw/CFL3D/build/$(CFLLIBSD) 
FFLAG_SPEC   = -O2   -g -ffast-math -w -ip -fno-alias -xHost -traceback -fpe0 -module /home/asc18/zjw/CFL3D/build/$(CFLLIBSD)

LLIBS        = -L/home/asc18/zjw/CFL3D/external/cgns/lib/ -lcgns
LLIBS_SEQ    = -L/home/asc18/zjw/CFL3D/external/cgns/lib/ -lcgns

CGNS_INCDIR  = -I/home/asc18/zjw/CFL3D/external/cgns/include
~~~

# 运行测试样例

结果文件为.out 和.res,请自行比对官网[Test Cases](https://cfl3d.larc.nasa.gov/Cfl3dv6/cfl3dv6_testcases.html)

以下为官网样例测试情况,可运行的样例使用shells中的对应脚本即可运行

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

