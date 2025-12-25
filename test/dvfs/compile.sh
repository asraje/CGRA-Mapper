clang-21 -emit-llvm -O3 -fno-unroll-loops -fno-vectorize -ffp-contract=off -o kernel.bc -c kernel.cpp
#llvm-dis fir.bc -o fir.ll
