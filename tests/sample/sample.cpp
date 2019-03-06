#include "korali.h"
#include "math.h"
#include "mpi.h"

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
	double sigma = 0.2;
  double mean[NDIMS];
  for (int i = 0; i < NDIMS; i++) mean[i] = 0.0;

	double mul = 1.0 / (sigma * sqrt(2*M_PI));
	double inExp = 0.0;

  for (int i = 0; i < NDIMS; i++)  { double xp = (x[i] - mean[i])/sigma; inExp += xp*xp; }

  return mul*exp(-0.5 * inExp );
}


int main(int argc, char* argv[])
{
  auto problem = Korali::DirectEvaluation(f_Gaussian, 982323);

  Korali::Parameter p;
  p.setBounds(-3.0, +3.0);
  p.setPriorDistribution("Uniform", -3.0, +3.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem, MPI_COMM_WORLD);
	Solver.setPopulationSize(20000);
	Solver.run();

	return 0;
}
