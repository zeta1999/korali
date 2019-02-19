#include "korali.h"
#include "auxiliar/auxiliar.hpp"

int main(int argc, char* argv[])
{
	const size_t nDim = 4;

  auto problem = Korali::Posterior(heat2DWrapper);
  Korali::Parameter intensity, width, xPos, yPos;

  intensity.setPriorDistribution("Uniform", 0.0, 50.0);
  intensity.setBounds(0.0, 50.0);
  intensity.setInitialX(10.0);
  intensity.setInitialStdDev(5.0);

  width.setPriorDistribution("Uniform", 0.0, 0.1);
  width.setBounds(0.0, 0.1);
  width.setInitialX(0.0);
  width.setInitialStdDev(0.01);

  xPos.setPriorDistribution("Uniform", 0.2, 0.6);
  xPos.setBounds(0.2, 0.6);
  xPos.setInitialX(0.5);
  xPos.setInitialStdDev(0.2);

  yPos.setPriorDistribution("Uniform", 0.2, 0.6);
  yPos.setBounds(0.2, 0.6);
  yPos.setInitialX(0.5);
  yPos.setInitialStdDev(0.2);

  problem.addParameter(intensity);
  problem.addParameter(width);
  problem.addParameter(xPos);
  problem.addParameter(yPos);

  problem.addReferenceData(0.6);

	problem.getEngine()->setStopMinDeltaX(1e-11);
	problem.getEngine()->setLambda(64);
	problem.solve();

	return 0;
}
