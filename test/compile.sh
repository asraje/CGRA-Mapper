clang -emit-llvm -fno-unroll-loops -O0 -o kernel.bc -c kernel.cpp
llvm-dis kernel.bc -o O0kernel.ll
#clang -emit-llvm -fno-unroll-loops -mllvm -force-vector-width=4 -O3 -o kernel.bc -c ./_matmul/src/matmul.c
