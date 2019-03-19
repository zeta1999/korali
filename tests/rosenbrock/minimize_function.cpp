#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
  auto problem = Korali::Direct([](double *x) {return -rosenbrock(x);});

  Korali::Parameter::Uniform p(-3.0, +3.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

  auto Solver = Korali::Solver::CMAES(&problem);
	Solver.setStopMinDeltaX(1e-11);
	Solver.setPopulationSize(128);
	Solver.run();

	return 0;
}
