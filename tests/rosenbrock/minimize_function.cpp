#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
 auto problem = Korali::Problem::Direct([](double *x) {return -rosenbrock(x);});

 Korali::Parameter::Uniform p(-3.0, +3.0);
 for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

 auto solver = Korali::Solver::CMAES(&problem);

 solver.setStopMinDeltaX(1e-11);
 solver.setPopulationSize(128);
 solver.setReportVerbosity(KORALI_NORMAL);
 solver.run();

 return 0;
}
