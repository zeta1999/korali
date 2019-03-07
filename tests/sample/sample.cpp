#include "korali.h"
#include "math.h"
#include "mpi.h"

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

#define NDIMS 2
double f_Rosenbrock(double *x)
{
  double s = 0.;
  for (int i = 0; i < NDIMS; ++i)
      s += 100.*pow(x[i+1]-x[i]*x[i], 2) + pow(x[i]-1., 2);
  return -s;
}

double f_Ackley(double *x)
{
   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (int i = 0; i < NDIMS; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   return a*exp(-b*sqrt(s1/NDIMS)) - exp(s2/NDIMS) + a + exp(1.);
}


double f_Gaussian(double *x)
{
	double mean[NDIMS] = {0.0, 0.0};
	double sigma[NDIMS*NDIMS] = {1.0, 0.0, 0.0, 1.0};
	double work[NDIMS];

  gsl_vector_view vals_view 	= gsl_vector_view_array(x, NDIMS);
  gsl_vector_view mean_view 	= gsl_vector_view_array(mean, NDIMS);
  gsl_matrix_view sigma_view 	= gsl_matrix_view_array(sigma, NDIMS,NDIMS);
  gsl_vector_view work_view 	= gsl_vector_view_array(work, NDIMS);

  gsl_matrix *L = gsl_matrix_alloc(NDIMS,NDIMS);
  gsl_matrix_memcpy(L, &sigma_view.matrix);
  gsl_linalg_cholesky_decomp(L);

  double res = 0.0; gsl_ran_multivariate_gaussian_log_pdf(&vals_view.vector, &mean_view.vector, L, &res, &work_view.vector);

  return res;
}


int main(int argc, char* argv[])
{
  auto problem = Korali::DirectEvaluation(f_Rosenbrock, 123315);

  Korali::Parameter p;
  p.setBounds(-15.0, +15.0);
  p.setPriorDistribution("Uniform", -15.0, +15.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem, MPI_COMM_WORLD);
	Solver.setPopulationSize(70000);
	Solver.run();

	return 0;
}
