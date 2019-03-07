#include "korali.h"
#include "model/gaussian.h"

int main(int argc, char* argv[])
{
	gaussian_init();

  auto problem = Korali::DirectEvaluation(gaussian);

  Korali::Parameter p;
  p.setBounds(-15.0, +15.0);
  p.setPriorDistribution("Uniform", -15.0, +15.0);
	for (int i = 0; i < NDIMS; i++) problem.addParameter(p);

  auto Solver = Korali::KoraliTMCMC(&problem);
	Solver.setPopulationSize(70000);
	Solver.setCovarianceScaling(0.05);
	Solver.run();

	return 0;
}
