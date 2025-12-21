opt -load-pass-plugin=../../build/src/libmapperPass.so -passes=mapperPass nonlinear_test.bc -o /dev/null
# dot -Tpng _Z6kernelPfS_.dot -o kernel.png
# clang -O3 -emit-llvm -fno-unroll-loops -fno-vectorize nonlinear_test.cpp -S -o nonlinear_test.ll