#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
 auto korali = Korali::Engine();

 korali["Seed"] = 0xC0FFEE;
 korali["Verbosity"] = "Detailed";

 korali["Problem"]["Type"] = "Direct";
 for (int i = 0; i < 4; i++)
 {
  korali["Problem"]["Variables"][i]["Name"] = "X" + std::to_string(i);
  korali["Problem"]["Variables"][i]["Type"] = "Computational";
  korali["Problem"]["Variables"][i]["Distribution"] = "Uniform";
  korali["Problem"]["Variables"][i]["Minimum"] = -32.0;
  korali["Problem"]["Variables"][i]["Maximum"] = +32.0;
 }

 korali["Conduit"]["Type"] = "Nonintrusive";

 korali["Solver"]["Method"] = "CMA-ES";
 korali["Solver"]["Lambda"] = 10;
 korali["Solver"]["Termination Criteria"]["Max Generations"]["Value"] = 100;
 korali["Solver"]["Termination Criteria"]["Min DeltaX"]["Value"] = 1e-12;
 
 korali.addModel([](Korali::ModelData& d) { printf("Hola\n"); });

 korali.run();

 return 0;
}
