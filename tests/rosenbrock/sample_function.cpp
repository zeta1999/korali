#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
  auto problem = Korali::DirectEvaluation([](double *x) {return -rosenbrock(x);});

  Korali::Parameter p;
  p.setBounds(-3.0, +3.0);
  p.setPriorDistribution("Uniform", -3.0, +3.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem);
	Solver.setPopulationSize(70000);
	Solver.setCovarianceScaling(0.2);
	Solver.run();

	return 0;
}
