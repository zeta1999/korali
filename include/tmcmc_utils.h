#ifndef TMCMC_UTILS_HPP
#define TMCMC_UTILS_HPP

#include "tmcmc_types.h"
#include <gsl/gsl_rng.h>

namespace Korali
{


//TODO: align with cmaes (cmaes_utils) (DW)

#include <sys/time.h>

// TODO: where should this go (DW)
static int g_nfeval;
static int l_nfeval = 0;
static int t_nfeval = 0;

int get_nfc();
int get_tfc();
void inc_nfc();
void reset_nfc();

int compar_desc(const void *p1, const void *p2);

double compute_sum(double *v, int n);

double compute_dot_product(double row_vector[], double vector[], int dim);

void compute_mat_product_vect(double *mat/*2D*/, double vect[], double res_vect[], double coef, int dim);

void inv_matrix(double *current_hessian/*2D*/, double *inv_hessian/*2D*/, int dim);

double scale_to_box(const double* point, double sc, const double* add_vec, const double *elbds, const double *eubds, int dims);

int in_rect(double *v1, double *v2, double *diam, double sc, int D);

int make_posdef(double *mat, int dim, int method);

void print_matrix(const char *name, double *x, int n);

void print_matrix_i(char *name, int *x, int n);

void print_matrix_2d(const char *name, double **x, int n1, int n2);


void multinomialrand(size_t K, unsigned int N, double q[], unsigned int nn[], gsl_rng* range);
int mvnrnd(double *mean, double *sigma, double *out, int N, gsl_rng* range);

} // namespace Korali

#endif //TMCMC_UTILS_HPP
