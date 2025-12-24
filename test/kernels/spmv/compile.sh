clang-21 -emit-llvm -O3 -fno-unroll-loops -o kernel.bc -c spmv.c
llvm-dis-21 kernel.bc -o kernel.ll
