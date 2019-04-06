#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
 auto korali = Korali::Engine([](double *x) {return -rosenbrock(x);});

 korali["Seed"] = 0xC0FFEE;
 korali["Verbosity"] = "Normal";

 for (int i = 0; i < NDIMS; i++)
 {
	 korali["Parameters"][i]["Distribution"] = "Uniform";
	 korali["Parameters"][i]["Minimum"] = -3.0;
	 korali["Parameters"][i]["Maximum"] = +3.0;
 }

 korali["Problem"]["Objective"] = "Direct Evaluation";

 korali["Solver"]["Method"] = "TMCMC";
 korali["Solver"]["CovarianceScaling"] = 0.04;
 korali["Solver"]["PopulationSize"] = 30000;

 return 0;
}
