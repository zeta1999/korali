#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
 auto korali = Korali::Engine([](double *x) {return -ackley(x);});

 korali["Seed"] = 0xC0FFEE;
 korali["Verbosity"] = "Normal";

 for (int i = 0; i < NDIMS; i++)
 {
	 korali["Parameters"][i]["Name"] = "X" + std::to_string(i);
	 korali["Parameters"][i]["Distribution"]["Type"] = "Uniform";
	 korali["Parameters"][i]["Distribution"]["Minimum"] = -32.0;
	 korali["Parameters"][i]["Distribution"]["Maximum"] = +32.0;
 }

 korali["Problem"]["Objective"] = "Direct Evaluation";

 korali["Solver"]["Method"] = "TMCMC";
 korali["Solver"]["Covariance Scaling"] = 0.02;
 korali["Solver"]["Population Size"] = 5000;
 korali["Solver"]["Burn In"] = 5;

 korali.run();

 return 0;
}
