# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o kernel.bc -c fft.c
llvm-dis kernel.bc -o kernel.ll
opt --loop-unroll --unroll-count=2 kernel.bc -o kernel_unroll.bc
#clang -emit-llvm -O3 -fno-unroll-loops -Rpass-analysis=loop-vectorize -o kernel.bc -c fft.c
llvm-dis kernel_unroll.bc -o kernel_unroll.ll
