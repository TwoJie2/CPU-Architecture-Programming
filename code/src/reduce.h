#ifndef REDUCE_H
#define REDUCE_H

void init_array(double *a, int n);
double sum_naive(const double *a, int n);
double sum_dual(const double *a, int n);
double sum_dual_unroll4(const double *a, int n);
double sum_pairwise(double *a, int n);

#endif
