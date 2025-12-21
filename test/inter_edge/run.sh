if command -v opt >/dev/null 2>&1; then
    opt -load-pass-plugin=../../build/src/libmapperPass.so -passes=mapperPass inter_edge_test.bc
else
    opt -load-pass-plugin=../../build/src/libmapperPass.so -passes=mapperPass inter_edge_test.bc
fi
