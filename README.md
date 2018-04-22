# CFL3D

### 备忘录 by huangh223
1. c_fhat 已改写完毕，但 `c_fhat_modified()` 未找到符合的算例进行测试。向量化已打开，需配合 build/cfl/libs/makefile 中的 `-fp_speculation=safe` 进行编译。
2. 现在 C 代码中对浮点类型的定义由宏 FTYPE 控制。复数类型应该可以用同样的方式配合 CMPLX 宏进行条件编译，但未测试。