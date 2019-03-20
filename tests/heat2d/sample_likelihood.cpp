#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
	heat2DInit(argc, argv);

  auto problem = Korali::Problem::Likelihood(heat2DSolver, 0xC0FFEE);

  Korali::Parameter::Uniform par0("Intensity", 10.0, 60.0);
  Korali::Parameter::Uniform par1("PosX",      0.0,  0.5);
  Korali::Parameter::Uniform par2("PosY",      0.6,  1.0);

  problem.addParameter(&par0);
  problem.addParameter(&par1);
  problem.addParameter(&par2);

  problem.setReferenceData(p.nPoints, p.refTemp);

  auto solver = Korali::Solver::TMCMC(&problem);

	solver.setPopulationSize(20000);
	solver.setCovarianceScaling(0.02);
	solver.run();

	return 0;
}
