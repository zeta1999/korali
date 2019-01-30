#include "engine_cmaes.hpp"
#include "korali.h"

extern "C" {
    #include "fitfun_tests.h"
}

int main(int argc, char* argv[])
{
	Korali::Prior P;

	Korali::GaussianDistribution g(2.0, 0.7, 15613);
	Korali::UniformDistribution u(0.3, 5.7, 658395);
	Korali::GammaDistribution m(0.3, 5.7, 1341);
	Korali::ExponentialDistribution e(2.1, 16424);

	P.addDimension(&g);
	P.addDimension(&u);
	P.addDimension(&m);
	P.addDimension(&e);

	P.initialize(4);

    for(int i = 0; i < 30; i++)
    {
      double* n = P.getRandomNumber();
      printf("Random Number: [%1.3f, %1.3f, %1.3f, %1.3f]\n", n[0], n[1], n[2], n[3]);
    }

//	Korali::Engine eng;
//
//    fitfun_initialize_simple("Ackley");
//    auto engine = CmaesEngine(&fitfun, "./");
//    engine.run();
}

double myFunction()
{
	return 0.0;
}
