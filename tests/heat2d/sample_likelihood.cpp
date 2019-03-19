#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
	heat2DInit(argc, argv);

  auto problem = Korali::Likelihood(heat2DSolver, 0xC0FFEE);

  Korali::Parameter::Uniform par0(10.0, 60.0);
  Korali::Parameter::Uniform par1(0.0,  0.5);
  Korali::Parameter::Uniform par2(0.6,  1.0);

  par0.setName("Intensity");
  par1.setName("PosX");
  par2.setName("PosY");

  par0.setInitialX(30.0);
	par1.setInitialX(0.25);
	par2.setInitialX(0.8);

  problem.addParameter(&par0);
  problem.addParameter(&par1);
  problem.addParameter(&par2);

  problem.setReferenceData(p.nPoints, p.refTemp);

  auto Solver = Korali::TMCMC(&problem);
	Solver.setPopulationSize(10000);
	Solver.setCovarianceScaling(0.02);
	Solver.run();

	return 0;
}
