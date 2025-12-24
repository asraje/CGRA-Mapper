clang-21 -emit-llvm -O3 -fno-unroll-loops -o kernel.bc -c gemm.c
llvm-dis-21 kernel.bc -o kernel.ll
opt-21 --loop-unroll --unroll-count=4 kernel.bc -o kernel_unroll.bc
llvm-dis-21 kernel_unroll.bc -o kernel_unroll.ll
