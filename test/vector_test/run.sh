#!/bin/bash
# Test script for vectorization detection using LLVM auto-vectorization
# This follows the VecPAC paper approach where LLVM auto-vectorizes loops

set -e

# Use opt to load the mapper pass
OPT=/opt/homebrew/opt/llvm/bin/opt
MAPPER_PASS=../../build/src/libmapperPass.so

if [ ! -f "$MAPPER_PASS" ]; then
    echo "Error: libmapperPass.so not found at $MAPPER_PASS"
    echo "Please build the project first:"
    echo "  cd ../../build && cmake -DLLVM_DIR=/opt/homebrew/opt/llvm/lib/cmake/llvm .. && make"
    exit 1
fi

# Compile with LLVM auto-vectorization enabled
# -mllvm -force-vector-width=4 forces vectorization with width 4
echo "=== Compiling vector_add.c with LLVM auto-vectorization ==="
clang -emit-llvm -O3 -fno-unroll-loops -mllvm -force-vector-width=4 -o kernel.bc -c vector_add.c

# Optionally disassemble to see the vectorized IR
/opt/homebrew/opt/llvm/bin/llvm-dis kernel.bc -o kernel.ll 2>/dev/null || true

# Run the mapper
echo ""
echo "=== Running CGRA Mapper ==="
$OPT -load-pass-plugin=$MAPPER_PASS -passes=mapperPass kernel.bc 2>&1 | grep -E "(Loop Selection|DFG generated|Output files)" || true

# Show results summary
echo ""
echo "=== Tile Type Summary ==="
echo ""
echo "VECTOR tile operations (for vector PEs):"
grep '"tile_type"  : "vector"' dfg.json -B4 | grep '"org_opt"' | sed 's/.*: "\(.*\)".*/  - \1/' | sort | uniq -c

echo ""
echo "SCALAR tile operations (for scalar PEs):"
grep '"tile_type"  : "scalar"' dfg.json -B4 | grep '"org_opt"' | sed 's/.*: "\(.*\)".*/  - \1/' | sort | uniq -c

echo ""
echo "=== Full dfg.json ==="
cat dfg.json
