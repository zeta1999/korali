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
	auto engine = CmaesEngine(4, f_Ackley);

	Korali::Prior P0;

	Korali::UniformDistribution u0(-6, 6, 45);
	Korali::UniformDistribution u1(-6, 6, 13513);
	Korali::UniformDistribution u2(-6, 6, 141);
	Korali::UniformDistribution u3(-6, 6, 4614);

	P0.addDimension(&u0);
	P0.addDimension(&u1);
	P0.addDimension(&u2);
	P0.addDimension(&u3);

	P0.initialize(1);

//    for(int i = 0; i < 30; i++)
//    {
//      double* n = P.getRandomNumber();
//      printf("Random Number: [%1.3f, %1.3f, %1.3f, %1.3f]\n", n[0], n[1], n[2], n[3]);
//    }

	engine.addBound(-32.0, +32.0);
	engine.addBound(-32.0, +32.0);
	engine.addBound(-32.0, +32.0);
	engine.addBound(-32.0, +32.0);

	engine.addPrior(&P0);

	engine.run();
}

double myFunction()
{
	return 0.0;
}

//	Korali::Engine eng;
//
//    fitfun_initialize_simple("Ackley");
//    auto engine = CmaesEngine(&fitfun, "./");
//    engine.run();
