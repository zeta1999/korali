#include "korali.h"
#include "model/ackley.h"

int main(int argc, char* argv[])
{
 auto problem = Korali::Problem::Direct([](double *x) {return -ackley(x);});

 Korali::Parameter::Uniform p(-32.0, +32.0);
 for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

 auto solver = Korali::Solver::CMAES(&problem);

 solver.setStopMinDeltaX(1e-11);
 solver.setPopulationSize(128);
 solver.run();

 return 0;
}
