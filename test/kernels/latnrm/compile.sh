clang-21 -emit-llvm -O3 -fno-unroll-loops -Rpass=loop-vectorize -o kernel.bc -c latnrm.c
llvm-dis-21 kernel.bc -o kernel.ll
