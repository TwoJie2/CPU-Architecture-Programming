#ifndef DOT_H
#define DOT_H

void init_matrix_vector(double *a, double *b, int n);
void dot_naive(const double *a, const double *b, double *sum, int n);
void dot_cache(const double *a, const double *b, double *sum, int n);
void dot_cache_unroll4(const double *a, const double *b, double *sum, int n);

#endif
