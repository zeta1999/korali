#include "korali.h"
#include "model/gaussian.h"

int main(int argc, char* argv[])
{
	gaussian_init();

	auto problem = Korali::Problem::Direct(gaussian);

	Korali::Parameter::Uniform p(-32.0, +32.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

  auto Solver = Korali::Solver::CMAES(&problem);
	Solver.setStopMinDeltaX(1e-11);
	Solver.setPopulationSize(128);
	Solver.run();

	return 0;
}
