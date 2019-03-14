#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
	heat2DInit(argc, argv);

  auto problem = Korali::Posterior(heat2DSolver);
  Korali::Parameter par0("Intensity");
  Korali::Parameter par1("PosX");
  Korali::Parameter par2("PosY");

  par0.setBounds(30.0, 45.0);
  par1.setBounds(0.1, 0.5);
  par2.setBounds(0.6, 1.0);

	par0.setPriorDistribution("Uniform", 30.0, 45.0);
	par1.setPriorDistribution("Uniform", 0.1, 0.5);
	par2.setPriorDistribution("Uniform", 0.6, 1.0);

  problem.addParameter(par0);
  problem.addParameter(par1);
  problem.addParameter(par2);

  problem.setReferenceData(p.nPoints, p.refTemp);

  auto Solver = Korali::TMCMC(&problem);
	Solver.setPopulationSize(1000);
	Solver.setCovarianceScaling(0.02);
	Solver.run();

	return 0;
}
