// Simple vector addition test to verify vectorization detection
// Compile with: clang -emit-llvm -O3 -mllvm -force-vector-width=4 -fno-unroll-loops -o kernel.bc -c vector_add.c

#define N 64

int kernel(int* A, int* B, int* C) {
    int sum = 0;

    // This loop should be vectorized with force-vector-width=4
    #pragma clang loop vectorize(enable) vectorize_width(4)
    for (int i = 0; i < N; i++) {
        C[i] = A[i] + B[i];
        sum += C[i];
    }

    return sum;
}

int main() {
    int A[N], B[N], C[N];

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = i * 2;
    }

    int result = kernel(A, B, C);
    return result;
}
