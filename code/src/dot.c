#include "dot.h"

void init_matrix_vector(double *a, double *b, int n) {
    for (int i = 0; i < n; ++i) {
        a[i] = 1.0 + (i % 7) * 0.125;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            b[i * n + j] = ((i * 13 + j * 7) % 97) * 0.25 + 1.0;
        }
    }
}

void dot_naive(const double *a, const double *b, double *sum, int n) {
    for (int i = 0; i < n; ++i) {
        double acc = 0.0;
        for (int j = 0; j < n; ++j) {
            acc += b[j * n + i] * a[j];
        }
        sum[i] = acc;
    }
}

void dot_cache(const double *a, const double *b, double *sum, int n) {
    for (int i = 0; i < n; ++i) {
        sum[i] = 0.0;
    }
    for (int j = 0; j < n; ++j) {
        const double aj = a[j];
        const double *row = b + j * n;
        for (int i = 0; i < n; ++i) {
            sum[i] += row[i] * aj;
        }
    }
}

void dot_cache_unroll4(const double *a, const double *b, double *sum, int n) {
    for (int i = 0; i < n; ++i) {
        sum[i] = 0.0;
    }
    for (int j = 0; j < n; ++j) {
        const double aj = a[j];
        const double *row = b + j * n;
        int i = 0;
        for (; i + 3 < n; i += 4) {
            sum[i]     += row[i]     * aj;
            sum[i + 1] += row[i + 1] * aj;
            sum[i + 2] += row[i + 2] * aj;
            sum[i + 3] += row[i + 3] * aj;
        }
        for (; i < n; ++i) {
            sum[i] += row[i] * aj;
        }
    }
}
