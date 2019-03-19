#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
  auto problem = Korali::Problem::Direct([](double *x) {return -ackley(x);});

  Korali::Parameter::Uniform p(-32.0, +32.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

  auto Solver = Korali::Solver::TMCMC(&problem);
	Solver.setPopulationSize(20000);
	Solver.setCovarianceScaling(0.2);
	Solver.run();

	return 0;
}
