#include "korali.h"
#include "mpi.h"

double f_Ackley(double *x, int N, void *data)
{
   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (int i = 0; i < N; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   return -a*exp(-b*sqrt(s1/N)) - exp(s2/N) + a + exp(1.);
}

int main(int argc, char* argv[])
{
  auto problem = Korali::Problem(f_Ackley);

  Korali::Parameter p;
  p.setPriorDistribution("Uniform", -32.0, +32.0);
  p.setBounds(-32.0, +32.0);
  p.setInitialX(5.0);
  p.setInitialStdDev(2.0);

	for (int i = 0; i < 4; i++)	problem.addParameter(p);

  auto Solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);
	Solver.setStopMinDeltaX(1e-11);
	Solver.setLambda(128);
	Solver.run();

	return 0;
}
