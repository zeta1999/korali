#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
 auto korali = Korali::Engine(0xC0FFEE);

 korali._problem = new Korali::Problem::Direct([](double *x) {return -rosenbrock(x);});

 Korali::Parameter::Uniform p(-3.0, +3.0);
 for (int i = 0; i < NDIMS; i++) korali.addParameter(&p);

 auto solver = new Korali::Solver::CMAES();
 solver->setStopMinDeltaX(1e-11);

 korali._solver = solver;
 korali.setReportVerbosity(KORALI_NORMAL);
 korali.setPopulationSize(128);
 korali.run();

 return 0;
}
