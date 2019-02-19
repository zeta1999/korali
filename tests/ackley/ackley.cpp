#include "korali.h"
#include "mpi.h"

void f_Ackley(double *x, int N, double *res) {
   int i;

   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (i = 0; i < N; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   *res = -a*exp(-b*sqrt(s1/N)) - exp(s2/N) + a + exp(1.);
}

int main(int argc, char* argv[])
{
  auto problem = Korali::Minimizer(f_Ackley);

  Korali::Parameter p;
  p.setPriorDistribution("Uniform", -32.0, +32.0);
  p.setBounds(-32.0, +32.0);
  p.setInitialX(5.0);
  p.setInitialStdDev(2.0);

  const size_t nDim = 4;

	for (int i = 0; i < nDim; i++)	problem.addParameter(p);

	auto solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);

	solver.setStopMinDeltaX(1e-11);
	solver.setLambda(128);

	solver.run();

	return 0;
}
