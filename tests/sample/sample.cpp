#include "korali.h"
#include "mpi.h"

#define NDIMS 4

double f_Ackley(double *x)
{
   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (int i = 0; i < NDIMS; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   return -a*exp(-b*sqrt(s1/NDIMS)) - exp(s2/NDIMS) + a + exp(1.);
}

int main(int argc, char* argv[])
{
  auto problem = Korali::DirectEvaluation(f_Ackley);

  Korali::Parameter p;
  p.setBounds(-32.0, +32.0);
  p.setPriorDistribution("Gaussian", 0.0, +8.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem, MPI_COMM_WORLD);
	Solver.setPopulationSize(5000);
	Solver.run();

	return 0;
}
