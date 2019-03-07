#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
  auto problem = Korali::DirectEvaluation([](double *x) {return -ackley(x);});

  Korali::Parameter p;
  p.setBounds(-32.0, +32.0);
  p.setPriorDistribution("Uniform", -32.0, +32.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem);
	Solver.setPopulationSize(20000);
	Solver.run();

	return 0;
}
