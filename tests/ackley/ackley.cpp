#include "korali.h"
#include "mpi.h"

double f_Ackley(double *x, int N) {
   int i;

   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;

   for (i = 0; i < N; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }

   return (a*exp(-b*sqrt(s1/N)) - exp(s2/N) + a + exp(1.));
}

int main(int argc, char* argv[])
{
  auto problem = Korali::Problem(4, f_Ackley, 53754);

	for (int i = 0; i < 4; i++)	problem[i]->setPriorDistribution("Uniform", -6.0, 6.0);
	for (int i = 0; i < 4; i++)	problem[i]->setBounds(-32.0, +32.0);
	for (int i = 0; i < 4; i++)	problem[i]->setInitialX(0.0);
	for (int i = 0; i < 4; i++)	problem[i]->setInitialStdDev(3.0);

	auto solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);

	solver.setStopMinDeltaX(1e-11);
	solver.setLambda(64);

	solver.run();

	return 0;
}
