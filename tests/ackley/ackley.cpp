#include "engine_cmaes.hpp"
#include "korali.h"

double f_Ackley(double *x, int N) {
   int i;
   double a = 20, b = .2, c = 2.*M_PI, s1 = 0., s2 = 0.;
   for (i = 0; i < N; ++i) {
       s1 += x[i]*x[i];
       s2 += cos(c*x[i]);
   }
   return -(-a*exp(-b*sqrt(s1/N)) - exp(s2/N) + a + exp(1.));
}

int main(int argc, char* argv[])
{
	auto korali = Korali::KoraliBase(4, f_Ackley, 53753);

	korali[0]->setPriorDistribution("Uniform", -6.0, 6.0);
	korali[1]->setPriorDistribution("Uniform", -6.0, 6.0);
	korali[2]->setPriorDistribution("Uniform", -6.0, 6.0);
	korali[3]->setPriorDistribution("Uniform", -6.0, 6.0);

	korali[0]->setBounds(-32.0, +32.0);
	korali[1]->setBounds(-32.0, +32.0);
	korali[2]->setBounds(-32.0, +32.0);
	korali[3]->setBounds(-32.0, +32.0);

	korali[0]->setInitialX(0.0);
	korali[1]->setInitialX(0.0);
	korali[2]->setInitialX(0.0);
	korali[3]->setInitialX(0.0);

	korali[0]->setInitialStdDev(3.0);
	korali[1]->setInitialStdDev(3.0);
	korali[2]->setInitialStdDev(3.0);
	korali[3]->setInitialStdDev(3.0);

	korali.setStopMinDeltaX(1e-11);
	korali.setLambda(128);

	Korali::UniformDistribution u0(-6, 6, 45);
	Korali::UniformDistribution u1(-6, 6, 13513);
	Korali::UniformDistribution u2(-6, 6, 141);
	Korali::UniformDistribution u3(-6, 6, 4614);

	auto engine = CmaesEngine(4, f_Ackley);

	engine.run();
}
