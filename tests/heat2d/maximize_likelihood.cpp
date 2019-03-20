#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
	heat2DInit(argc, argv);

  auto problem = Korali::Problem::Likelihood(heat2DSolver);

  Korali::Parameter::Uniform par0("Intensity", 10.0, 60.0);
  Korali::Parameter::Uniform par1("PosX",      0.0,  0.5);
  Korali::Parameter::Uniform par2("PosY",      0.6,  1.0);

  problem.addParameter(&par0);
  problem.addParameter(&par1);
  problem.addParameter(&par2);

  problem.setReferenceData(p.nPoints, p.refTemp);

  auto solver = Korali::Solver::CMAES(&problem);

  solver["Intensity"]->setInitialValue(30.0);
  solver["PosX"]->setInitialValue(0.25);
  solver["PosY"]->setInitialValue(0.8);
	solver.setStopMinDeltaX(1e-7);
	solver.setPopulationSize(32);
	solver.run();

	return 0;
}
