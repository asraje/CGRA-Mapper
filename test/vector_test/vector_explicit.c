// Explicit vector operations using SIMD intrinsics
// This guarantees vectorized operations without scalar fallback

#include <arm_neon.h>

#define N 16

// Use explicit NEON vector intrinsics
int kernel(int* A, int* B, int* C) {
    int32x4_t sum = vdupq_n_s32(0);

    // Process 4 elements at a time using explicit vector ops
    for (int i = 0; i < N; i += 4) {
        int32x4_t a = vld1q_s32(&A[i]);  // Vector load
        int32x4_t b = vld1q_s32(&B[i]);  // Vector load
        int32x4_t c = vaddq_s32(a, b);   // Vector add
        vst1q_s32(&C[i], c);             // Vector store
        sum = vaddq_s32(sum, c);         // Accumulate
    }

    // Reduce vector to scalar
    return vaddvq_s32(sum);
}

int main() {
    int A[N], B[N], C[N];

    for (int i = 0; i < N; i++) {
        A[i] = i;
        B[i] = i * 2;
    }

    return kernel(A, B, C);
}
