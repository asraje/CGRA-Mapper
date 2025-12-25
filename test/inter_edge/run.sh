if command -v opt-21 >/dev/null 2>&1; then
    opt-21 --load-pass-plugin=../../build/src/libmapperPass.so --passes="mapperPass" inter_edge_test.bc
else
    opt-21 --load-pass-plugin=../../build/src/libmapperPass.so --passes="mapperPass" inter_edge_test.bc
fi
