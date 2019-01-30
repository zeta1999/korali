#include "engine_cmaes.hpp"
#include "korali.h"

extern "C" {
    #include "fitfun_tests.h"
}

int main(int argc, char* argv[])
{
	Korali::Prior P0;
	Korali::Prior P1;
	Korali::Prior P2;
	Korali::Prior P3;

	Korali::UniformDistribution u0(-6, 6, 0);
	Korali::UniformDistribution u1(-6, 6, 0);
	Korali::UniformDistribution u2(-6, 6, 0);
	Korali::UniformDistribution u3(-6, 6, 0);

	P0.addDimension(&u0);
	P1.addDimension(&u1);
	P2.addDimension(&u2);
	P3.addDimension(&u3);

	P0.initialize(1);
	P1.initialize(1);
	P2.initialize(1);
	P3.initialize(1);

//    for(int i = 0; i < 30; i++)
//    {
//      double* n = P.getRandomNumber();
//      printf("Random Number: [%1.3f, %1.3f, %1.3f, %1.3f]\n", n[0], n[1], n[2], n[3]);
//    }

	fitfun_initialize_simple("Ackley");
	auto engine = CmaesEngine(&fitfun, "./");

	engine.addPrior(&P0);
	engine.addPrior(&P1);
	engine.addPrior(&P2);
	engine.addPrior(&P3);

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
