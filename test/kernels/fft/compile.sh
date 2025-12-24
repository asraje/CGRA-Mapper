clang-21 -emit-llvm -O3 -fno-unroll-loops -o kernel.bc -c fft.c
llvm-dis-21 kernel.bc -o kernel.ll
opt-21 --loop-unroll --unroll-count=2 kernel.bc -o kernel_unroll.bc
#clang-21 -emit-llvm -O3 -fno-unroll-loops -Rpass-analysis=loop-vectorize -o kernel.bc -c fft.c
llvm-dis-21 kernel_unroll.bc -o kernel_unroll.ll
