# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -Rpass=loop-vectorize -o kernel.bc -c latnrm.c
llvm-dis kernel.bc -o kernel.ll
