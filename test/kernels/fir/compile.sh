clang-21 -emit-llvm -fno-unroll-loops -O3 -o kernel.bc -c fir.cpp
llvm-dis-21 kernel.bc -o kernel.ll
opt-21 --loop-unroll --unroll-count=4 kernel.bc -o kernel_unroll.bc
llvm-dis-21 kernel_unroll.bc -o kernel_unroll.ll
