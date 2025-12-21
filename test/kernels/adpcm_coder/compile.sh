# Enable LLVM auto-vectorization with vector width 4
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o adpcm.bc -c adpcm.c
#llvm-dis fir.bc -o fir.ll
