# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o bf.bc -c bf_test.c
#llvm-dis bf.bc -o bf.ll
