# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -fno-unroll-loops -O3 -mllvm -force-vector-width=4 -o pool.bc -c pool.c
#llvm-dis fir.bc -o fir.ll
