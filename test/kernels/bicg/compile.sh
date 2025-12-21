# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -Rpass-analysis=loop-vectorize -o kernel.bc -c bicg.c
llvm-dis kernel.bc -o kernel.ll
