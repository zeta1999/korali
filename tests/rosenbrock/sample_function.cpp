#include "korali.h"
#include "model/rosenbrock.h"

int main(int argc, char* argv[])
{
 auto problem = Korali::Problem::Direct([](double *x) {return -rosenbrock(x);}, 8500);

 Korali::Parameter::Uniform p(-3.0, +3.0);
 for (int i = 0; i < NDIMS; i++) problem.addParameter(&p);

 auto solver = Korali::Solver::TMCMC(&problem);

 solver.setPopulationSize(30000);
 solver.setCovarianceScaling(0.04);
 solver.run();

 return 0;
}
