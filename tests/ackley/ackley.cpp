#include "engine_cmaes.hpp"
#include "korali.h"

extern "C" {
    #include "fitfun_tests.h"
}

int main(int argc, char* argv[])
{
	Korali::GaussianDistribution g(2.0, 0.7, 15613);
	Korali::UniformDistribution u(0.3, 5.7, 658395);
	Korali::GammaDistribution m(0.3, 5.7, 1341);
	Korali::ExponentialDistribution e(2.1, 16424);

//	p.AddGaussianDistribution(3.0, 1.0); // X
//	p.AddUniformDistribution(3.0, 1.0); // Y
//	p.AddGammaDistribution(5.0, 3.0); // Z

//	Korali::TMCMC engine;
//
//	engine.AddPrior(p);
//	engine.SetFitFunction(myFunction);
//
//	engine.run();
//
    for(int i = 0; i < 30; i++) printf("Random Number: %f\n", e.getRandomNumber());

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
