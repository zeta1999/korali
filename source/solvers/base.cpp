#include "solvers/base.h"
#include "conduits/upcxx.h"
#include "conduits/single.h"
#include "conduits/openmp.h"

Korali::Solver::Base::Base(Korali::Problem::Base* problem)
{
 _problem = problem;
 _currentGeneration = 0;
 _sampleCount = 1000;
 _maxGens = 200;
 _verbosity = KORALI_NORMAL;
 _reportFrequency = 1;
 N = _problem->_parameterCount;
}

void Korali::Solver::Base::run()
{
 _problem->initializeParameters();
 auto printSyntax = [](){fprintf(stderr, "[Korali] Use $export KORALI_CONDUIT={single,openmp,upcxx,mpi} to select a conduit.\n");};

 const char* env_conduit = std::getenv("KORALI_CONDUIT");
 if(env_conduit == NULL)
 {
  fprintf(stderr, "[Korali] Error: No sampling conduit was selected.\n");
  printSyntax();
  exit(-1);
 }

 std::string conduitString = env_conduit;

 bool recognized = false;

 if (conduitString == "single") { _conduit = new Korali::Conduit::Single(this); recognized = true; }

 if (conduitString == "upcxx")
  {
   #ifdef _KORALI_USE_UPCXX
    _conduit = new Korali::Conduit::UPCXX(this);  recognized = true;
   #else
    fprintf(stderr, "[Korali] Error: UPC++ conduit is not properly configured.\n");
    printSyntax();
    fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support UPC++.\n");
    exit(-1);
   #endif
  }

 if (conduitString == "openmp")
 {
  #ifdef _KORALI_USE_OPENMP
   _conduit = new Korali::Conduit::OpenMP(this); recognized = true;
  #else
   fprintf(stderr, "[Korali] Error: OpenMP conduit is not properly configured.\n");
   printSyntax();
   fprintf(stderr, "[Korali] Reinstall Korali with the proper configuration to support openMP.\n");
   exit(-1);
  #endif
 }

 if (recognized == false)
 {
   fprintf(stderr, "[Korali] Error: Unrecognized conduit '%s' selected.\n", conduitString.c_str());
   printSyntax();
   exit(-1);
 }

 _problem->_conduit = _conduit;
 _conduit->initialize();
}

