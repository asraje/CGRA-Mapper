#!/bin/bash
# Compile WITH vectorization enabled to test vectorization detection
# The -force-vector-width=4 forces the loop vectorizer to use vector width of 4

clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o kernel.bc -c vector_add.c

# Optionally disassemble to see the vectorized IR
llvm-dis kernel.bc -o kernel.ll
echo "Generated kernel.bc and kernel.ll"
echo "Check kernel.ll for vector operations like <4 x i32>"
