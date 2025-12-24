opt-21 --load-pass-plugin=../../build/src/libmapperPass.so --passes="mapperPass" nonlinear_test.bc
# dot -Tpng _Z6kernelPfS_.dot -o kernel.png
# clang-21 -O3 -emit-llvm -fno-unroll-loops -fno-vectorize nonlinear_test.cpp -S -o nonlinear_test.ll