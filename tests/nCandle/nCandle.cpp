#include "korali.h"
#include "auxiliar/auxiliar.hpp"

int main(int argc, char* argv[])
{
	const size_t nDim = 4;

  auto problem = Korali::Problem("Maximizer", nDim, heat2DWrapper, 53754);

  problem[0]->setPriorDistribution("Uniform", 0.0, 50.0);
	problem[0]->setBounds(0.0, 50.0);
	problem[0]->setInitialX(10.0);
	problem[0]->setInitialStdDev(5.0);

	problem[1]->setPriorDistribution("Uniform", 0.0, 0.1);
	problem[1]->setBounds(0.0, 0.1);
	problem[1]->setInitialX(0.0);
	problem[1]->setInitialStdDev(0.01);

	problem[2]->setPriorDistribution("Uniform", 0.2, 0.6);
	problem[2]->setBounds(0.2, 0.6);
	problem[2]->setInitialX(0.5);
	problem[2]->setInitialStdDev(0.2);

	problem[3]->setPriorDistribution("Uniform", 0.2, 0.6);
	problem[3]->setBounds(0.2, 0.6);
	problem[3]->setInitialX(0.5);
	problem[3]->setInitialStdDev(0.2);

	auto solver = Korali::KoraliCMAES(&problem, MPI_COMM_WORLD);

	solver.setStopMinDeltaX(1e-11);
	solver.setLambda(64);

	solver.run();

	return 0;
}
