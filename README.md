# CFL3D

### 备忘录 by huangh223
1. 现在 fhat.F 已经可以与 c_fhat.c 进行混合编译。Makefile 主要的改动是 build/cfl/libs/makefile
2. 现在 C 代码中对浮点类型的定义由宏 FTYPE 控制。复数类型应该可以用同样的方式配合 CMPLX 宏进行条件编译