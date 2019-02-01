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

	for (int i = 0; i < 4; i++)	korali[i]->setPriorDistribution("Uniform", -6.0, 6.0);
	for (int i = 0; i < 4; i++)	korali[i]->setBounds(-32.0, +32.0);
	for (int i = 0; i < 4; i++)	korali[i]->setInitialX(0.0);
	for (int i = 0; i < 4; i++)	korali[i]->setInitialStdDev(3.0);

	korali.setStopMinDeltaX(1e-11);
	korali.setLambda(128);

	korali.run();
}
