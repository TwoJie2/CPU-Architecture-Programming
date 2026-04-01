#include "reduce.h"

void init_array(double *a, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = 1.0 + (i % 11) * 0.0625;
    }
}

double sum_naive(const double *a, int n) {
    double sum = 0.0;
    for (int i = 0; i < n; ++i) {
        sum += a[i];
    }
    return sum;
}

double sum_dual(const double *a, int n) {
    double s0 = 0.0, s1 = 0.0;
    int i = 0;
    for (; i + 1 < n; i += 2) {
        s0 += a[i];
        s1 += a[i + 1];
    }
    for (; i < n; ++i) {
        s0 += a[i];
    }
    return s0 + s1;
}

double sum_dual_unroll4(const double *a, int n) {
    double s0 = 0.0, s1 = 0.0, s2 = 0.0, s3 = 0.0;
    int i = 0;
    for (; i + 3 < n; i += 4) {
        s0 += a[i];
        s1 += a[i + 1];
        s2 += a[i + 2];
        s3 += a[i + 3];
    }
    for (; i < n; ++i) {
        s0 += a[i];
    }
    return (s0 + s1) + (s2 + s3);
}

double sum_pairwise(double *a, int n) {
    int m = n;
    while (m > 1) {
        int half = m / 2;
        for (int i = 0; i < half; ++i) {
            a[i] = a[2 * i] + a[2 * i + 1];
        }
        if (m & 1) {
            a[half] = a[m - 1];
            m = half + 1;
        } else {
            m = half;
        }
    }
    return a[0];
}
