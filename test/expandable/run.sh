opt -load-pass-plugin=../../build/src/libmapperPass.so -passes=mapperPass kernel.bc | tee trace.log
