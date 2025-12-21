# opt -load-pass-plugin=../../../build/src/libmapperPass.so -passes=mapperPass kernel_unroll.bc
opt -load-pass-plugin=../../../build/src/libmapperPass.so -passes=mapperPass kernel.bc
