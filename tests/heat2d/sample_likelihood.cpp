#include "model/heat2d.hpp"
#include "korali.h"

int main(int argc, char* argv[])
{
 heat2DInit(argc, argv);

 auto korali = Korali::Engine(heat2DSolver);

 korali["Seed"] = 0xC0FFEE;
 korali["Verbosity"] = "Normal";

 korali["Parameters"][0]["Name"] = "Intensity";
 korali["Parameters"][0]["Distribution"]["Type"] = "Uniform";
 korali["Parameters"][0]["Distribution"]["Minimum"] = 10.0;
 korali["Parameters"][0]["Distribution"]["Maximum"] = 60.0;

 korali["Parameters"][1]["Name"] = "PosX";
 korali["Parameters"][1]["Distribution"]["Type"] = "Uniform";
 korali["Parameters"][1]["Distribution"]["Minimum"] = 0.0;
 korali["Parameters"][1]["Distribution"]["Maximum"] = 0.5;

 korali["Parameters"][2]["Name"] = "PosY";
 korali["Parameters"][2]["Distribution"]["Type"] = "Uniform";
 korali["Parameters"][2]["Distribution"]["Minimum"] = 0.6;
 korali["Parameters"][2]["Distribution"]["Maximum"] = 1.0;

 korali["Problem"]["Objective"] = "Likelihood";
 for (int i = 0; i < p.nPoints; i++)
	korali["Problem"]["Reference Data"] += p.refTemp[i];

 korali["Solver"]["Method"] = "TMCMC";
 korali["Solver"]["Covariance Scaling"] = 0.02;
 korali["Solver"]["Population Size"] = 3000;

 korali.run();

 return 0;
}
