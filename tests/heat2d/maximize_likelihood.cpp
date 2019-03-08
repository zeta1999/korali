#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
	heat2DInit(argc, argv);

  auto problem = Korali::Likelihood(heat2DSolver);
  Korali::Parameter par0("Intensity"); par0.setBounds(0.0, 50.0); problem.addParameter(par0);
  Korali::Parameter par1("PosX");      par1.setBounds(0.0, 1.0);  problem.addParameter(par1);
  Korali::Parameter par2("PosY");      par2.setBounds(0.0, 1.0);  problem.addParameter(par2);

  problem.setReferenceData(p.nPoints, p.refTemp);

  auto Solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);
	Solver.setStopMinDeltaX(1e-7);
	Solver.setLambda(32);
	Solver.run();

	return 0;
}
