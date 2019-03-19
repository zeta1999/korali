#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
  auto problem = Korali::Direct([](double *x) {return -ackley(x);});

  Korali::Parameter::Uniform p(-32.0, +32.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

  auto Solver = Korali::CMAES(&problem);
	Solver.setStopMinDeltaX(1e-11);
	Solver.setPopulationSize(128);
	Solver.run();

	return 0;
}
