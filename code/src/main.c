#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "dot.h"
#include "reduce.h"
#include "timer.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#define EPS 1e-9
#define MIN_BENCH_SECONDS 0.20

static double absd(double x) { return x < 0.0 ? -x : x; }

static int nearly_equal(double x, double y) {
    double diff = absd(x - y);
    double scale = absd(x) > absd(y) ? absd(x) : absd(y);
    if (scale < 1.0) scale = 1.0;
    return diff <= EPS * scale;
}

static void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (!p) {
        fprintf(stderr, "malloc failed for %zu bytes\n", size);
        exit(1);
    }
    return p;
}

static void ensure_results_dir(void) {
    if (MKDIR("results") != 0 && errno != EEXIST) {
        fprintf(stderr, "failed to create results directory\n");
        exit(1);
    }
}

static int choose_repeats_for_dot(int n) {
    long long work = 2LL * n * n;
    int repeats = (int)(150000000LL / (work > 0 ? work : 1));
    return repeats < 1 ? 1 : repeats;
}

static int choose_repeats_for_sum(int n) {
    int repeats = 200000000 / (n > 0 ? n : 1);
    return repeats < 1 ? 1 : repeats;
}

static int verify_dot(void) {
    int n = 17;
    double *a = (double *)xmalloc((size_t)n * sizeof(double));
    double *b = (double *)xmalloc((size_t)n * (size_t)n * sizeof(double));
    double *s0 = (double *)xmalloc((size_t)n * sizeof(double));
    double *s1 = (double *)xmalloc((size_t)n * sizeof(double));
    double *s2 = (double *)xmalloc((size_t)n * sizeof(double));
    int ok = 1;
    init_matrix_vector(a, b, n);
    dot_naive(a, b, s0, n);
    dot_cache(a, b, s1, n);
    dot_cache_unroll4(a, b, s2, n);
    for (int i = 0; i < n; ++i) {
        if (!nearly_equal(s0[i], s1[i]) || !nearly_equal(s0[i], s2[i])) {
            ok = 0;
            fprintf(stderr, "dot mismatch at %d: %.12f %.12f %.12f\n", i, s0[i], s1[i], s2[i]);
            break;
        }
    }
    free(a); free(b); free(s0); free(s1); free(s2);
    return ok;
}

static int verify_sum(void) {
    int n = 1025;
    double *a = (double *)xmalloc((size_t)n * sizeof(double));
    double *w1 = (double *)xmalloc((size_t)n * sizeof(double));
    init_array(a, n);
    memcpy(w1, a, (size_t)n * sizeof(double));
    double s0 = sum_naive(a, n);
    double s1 = sum_dual(a, n);
    double s2 = sum_dual_unroll4(a, n);
    double s3 = sum_pairwise(w1, n);
    free(a); free(w1);
    return nearly_equal(s0, s1) && nearly_equal(s0, s2) && nearly_equal(s0, s3);
}

static double bench_dot(void (*fn)(const double *, const double *, double *, int),
                        const double *a, const double *b, double *sum, int n, int *used_repeats) {
    int repeats = choose_repeats_for_dot(n);
    double t0 = 0.0, t1 = 0.0;
    do {
        t0 = now_seconds();
        for (int r = 0; r < repeats; ++r) fn(a, b, sum, n);
        t1 = now_seconds();
        if (t1 - t0 < MIN_BENCH_SECONDS) repeats *= 2;
        else break;
    } while (repeats < (1 << 30));
    *used_repeats = repeats;
    return (t1 - t0) / repeats;
}

static double bench_sum(double (*fn)(const double *, int), const double *a, int n, int *used_repeats) {
    int repeats = choose_repeats_for_sum(n);
    volatile double sink = 0.0;
    double t0 = 0.0, t1 = 0.0;
    do {
        t0 = now_seconds();
        for (int r = 0; r < repeats; ++r) sink += fn(a, n);
        t1 = now_seconds();
        if (t1 - t0 < MIN_BENCH_SECONDS) repeats *= 2;
        else break;
    } while (repeats < (1 << 30));
    *used_repeats = repeats;
    if (sink == -1.0) puts("impossible");
    return (t1 - t0) / repeats;
}

static double bench_pairwise(const double *a, int n, int *used_repeats) {
    int repeats = choose_repeats_for_sum(n);
    double *work = (double *)xmalloc((size_t)n * sizeof(double));
    volatile double sink = 0.0;
    double t0 = 0.0, t1 = 0.0;
    do {
        t0 = now_seconds();
        for (int r = 0; r < repeats; ++r) {
            memcpy(work, a, (size_t)n * sizeof(double));
            sink += sum_pairwise(work, n);
        }
        t1 = now_seconds();
        if (t1 - t0 < MIN_BENCH_SECONDS) repeats *= 2;
        else break;
    } while (repeats < (1 << 30));
    *used_repeats = repeats;
    free(work);
    if (sink == -1.0) puts("impossible");
    return (t1 - t0) / repeats;
}

static void run_dot_suite(const char *tag) {
    const int sizes[] = {64, 128, 256, 384, 512, 768, 1024, 1536, 2048};
    const int count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    char basic_name[256], adv_name[256];
    snprintf(basic_name, sizeof(basic_name), "results/dot_basic_%s.csv", tag);
    snprintf(adv_name, sizeof(adv_name), "results/dot_advanced_%s.csv", tag);
    FILE *fb = fopen(basic_name, "w");
    FILE *fa = fopen(adv_name, "w");
    if (!fb || !fa) {
        fprintf(stderr, "cannot open output csv files\n");
        exit(1);
    }
    fprintf(fb, "n,naive_ms,cache_ms,speedup,repeats_naive,repeats_cache\n");
    fprintf(fa, "n,naive_ms,cache_ms,cache_unroll4_ms,speedup_cache,speedup_unroll4,repeats_naive,repeats_cache,repeats_unroll4\n");
    puts("\n[矩阵-向量内积]");
    puts("n\tnaive\tcache\tcache_unroll4\tspeedup(cache)\tspeedup(unroll4)");
    for (int k = 0; k < count; ++k) {
        int n = sizes[k];
        double *a = (double *)xmalloc((size_t)n * sizeof(double));
        double *b = (double *)xmalloc((size_t)n * (size_t)n * sizeof(double));
        double *sum = (double *)xmalloc((size_t)n * sizeof(double));
        int r0 = 0, r1 = 0, r2 = 0;
        init_matrix_vector(a, b, n);
        double t0 = bench_dot(dot_naive, a, b, sum, n, &r0);
        double t1 = bench_dot(dot_cache, a, b, sum, n, &r1);
        double t2 = bench_dot(dot_cache_unroll4, a, b, sum, n, &r2);
        printf("%d\t%.6f\t%.6f\t%.6f\t%.3f\t%.3f\n", n, t0*1000.0, t1*1000.0, t2*1000.0, t0/t1, t0/t2);
        fprintf(fb, "%d,%.6f,%.6f,%.6f,%d,%d\n", n, t0*1000.0, t1*1000.0, t0/t1, r0, r1);
        fprintf(fa, "%d,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d\n", n, t0*1000.0, t1*1000.0, t2*1000.0, t0/t1, t0/t2, r0, r1, r2);
        free(a); free(b); free(sum);
    }
    fclose(fb); fclose(fa);
}

static void run_sum_suite(const char *tag) {
    const int sizes[] = {1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216};
    const int count = (int)(sizeof(sizes) / sizeof(sizes[0]));
    char basic_name[256], adv_name[256];
    snprintf(basic_name, sizeof(basic_name), "results/sum_basic_%s.csv", tag);
    snprintf(adv_name, sizeof(adv_name), "results/sum_advanced_%s.csv", tag);
    FILE *fb = fopen(basic_name, "w");
    FILE *fa = fopen(adv_name, "w");
    if (!fb || !fa) {
        fprintf(stderr, "cannot open output csv files\n");
        exit(1);
    }
    fprintf(fb, "n,naive_ms,dual_ms,speedup,repeats_naive,repeats_dual\n");
    fprintf(fa, "n,naive_ms,dual_ms,dual_unroll4_ms,pairwise_ms,speedup_dual,speedup_dual_unroll4,speedup_pairwise,repeats_naive,repeats_dual,repeats_dual_unroll4,repeats_pairwise\n");
    puts("\n[n 个数求和]");
    puts("n\tnaive\tdual\tdual_unroll4\tpairwise\tspd(dual)\tspd(unroll4)");
    for (int k = 0; k < count; ++k) {
        int n = sizes[k];
        double *a = (double *)xmalloc((size_t)n * sizeof(double));
        int r0 = 0, r1 = 0, r2 = 0, r3 = 0;
        init_array(a, n);
        double t0 = bench_sum(sum_naive, a, n, &r0);
        double t1 = bench_sum(sum_dual, a, n, &r1);
        double t2 = bench_sum(sum_dual_unroll4, a, n, &r2);
        double t3 = bench_pairwise(a, n, &r3);
        printf("%d\t%.6f\t%.6f\t%.6f\t%.6f\t%.3f\t%.3f\n", n, t0*1000.0, t1*1000.0, t2*1000.0, t3*1000.0, t0/t1, t0/t2);
        fprintf(fb, "%d,%.6f,%.6f,%.6f,%d,%d\n", n, t0*1000.0, t1*1000.0, t0/t1, r0, r1);
        fprintf(fa, "%d,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%d,%d,%d,%d\n", n, t0*1000.0, t1*1000.0, t2*1000.0, t3*1000.0, t0/t1, t0/t2, t0/t3, r0, r1, r2, r3);
        free(a);
    }
    fclose(fb); fclose(fa);
}

int main(int argc, char **argv) {
    const char *tag = "O2_default";
    if (argc == 3 && strcmp(argv[1], "--tag") == 0) tag = argv[2];
    ensure_results_dir();
    puts("Lab1 CPU architecture experiment");
    if (!verify_dot()) {
        fputs("dot verification failed\n", stderr);
        return 1;
    }
    if (!verify_sum()) {
        fputs("sum verification failed\n", stderr);
        return 1;
    }
    puts("Correctness check passed.");
    run_dot_suite(tag);
    run_sum_suite(tag);
    printf("\nAll csv files have been written into .\\results (tag = %s).\n", tag);
    return 0;
}
