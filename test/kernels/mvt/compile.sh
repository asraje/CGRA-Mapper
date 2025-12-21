# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o kernel.bc -c mvt.c
llvm-dis kernel.bc -o kernel.ll
opt --loop-unroll --unroll-count=4 kernel.bc -o kernel_unroll.bc
llvm-dis kernel_unroll.bc -o kernel_unroll.ll
