#include <cassert>
#include <math.h>
#include <limits>

#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_cblas.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_linalg.h>


#include "tmcmc_utils.h"

namespace Korali
{

int in_rect(double *v1, double *v2, double *diam, double sc, int D)
{
    int d;
    for (d = 0; d < D; ++d) {
        if (fabs(v1[d]-v2[d]) > sc*diam[d]) return 0;
    }
    return 1;
}


int compar_desc(const void* p1, const void* p2)
{
    int dir = +1;   /* -1: ascending order, +1: descending order */
    sort_t *s1 = (sort_t *) p1;
    sort_t *s2 = (sort_t *) p2;

    if (s1->nsel < s2->nsel) return dir;
    if (s1->nsel > s2->nsel) return -dir;

    return 0;
}

double compute_sum(double *v, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++) s += v[i];
    return s;
}


double compute_dot_product(double row_vector[], double vector[], int dim)
{
	double sum = 0.0;
	for(int row=0; row<dim; row++) sum += row_vector[row] * vector[row];
  
	return sum;
}


void compute_mat_product_vect(double *mat/*2D*/, double vect[], double res_vect[], double coef, int dim)
{
    int row, column;
    double current_dot_product;

	for(row=0; row<dim; ++row){
		current_dot_product = 0.0;
        for(column=0; column<dim; ++column) current_dot_product += mat[row*dim+column] * vect[column];
        res_vect[row] = coef * current_dot_product;
    }
    return;
}


void inv_matrix(double *current_hessian/*2D*/, double *inv_hessian/*2D*/, int dim)
{
    gsl_matrix_view m   = gsl_matrix_view_array(current_hessian, dim, dim);
    gsl_matrix_view inv = gsl_matrix_view_array(inv_hessian, dim, dim);
    gsl_permutation * p = gsl_permutation_alloc (dim);

    int s;
    gsl_linalg_LU_decomp (&m.matrix, p, &s);
    gsl_linalg_LU_invert (&m.matrix, p, &inv.matrix);

    gsl_permutation_free (p);
    return;
}


double scale_to_box(const double* point, double sc, const double* add_vec, const double *elbds, const double *eubds, int dims)
{
	double pp[dims];
	for(int i=0; i<dims; ++i) pp[i] = point[i]+sc*add_vec[i];

	sc = fabs(sc);
	double c;
	for (int l=0; l<dims; l++)
	{
		if (pp[l]<elbds[l])
		{
			c = fabs( (elbds[l]-point[l]) / add_vec[l] );
			sc = fmin(sc,c);
		}
		if (pp[l]>eubds[l])
		{
			c = fabs( (point[l]-eubds[l]) / add_vec[l] );
			sc = fmin(sc,c);
		}
	}
	return sc;
}


void print_matrix(const char *name, double *x, int n)
{
    printf("\n%s =\n\n", name);
    for (int i = 0; i < n; ++i) printf("   %20.15lf\n", x[i]);
    printf("\n");
}

void print_matrix_i(char *title, int *v, int n)
{
    printf("\n%s =\n\n", title);
    for (int i = 0; i < n; i++) printf("  %8d\n", v[i]);
    printf("\n");
}


void print_matrix_2d(const char *name, double **x, int n1, int n2)
{
    printf("\n%s =\n\n", name);
    for (int i = 0; i < n1; i++) {
        for (int j = 0; j < n2; j++) {
            printf("   %20.15lf", x[i][j]);
        }
        printf("\n");
    }
    printf("\n");

}

void multinomialrand(size_t K, unsigned int N, double q[], unsigned int nn[], gsl_rng* range)
{
    gsl_ran_multinomial (range, K, N, q, nn);

    return;
}


int mvnrnd(double *mean, double *sigma, double *out, int N, gsl_rng* range)
{
    int res;

    gsl_vector_view mean_view 	= gsl_vector_view_array(mean, N);
    gsl_matrix_view sigma_view 	= gsl_matrix_view_array(sigma, N,N);
    gsl_vector_view out_view 	= gsl_vector_view_array(out, N);

    gsl_matrix *L = gsl_matrix_alloc(N,N);
    gsl_matrix_memcpy( L, &sigma_view.matrix);
    gsl_linalg_cholesky_decomp( L );


	res = gsl_ran_multivariate_gaussian( range, &mean_view.vector, L, &out_view.vector);

    return res;
}

double uniformrand(double a, double b, gsl_rng* range)
{
    return gsl_ran_flat( range, a, b );
}



} //namespace
