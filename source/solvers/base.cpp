#include "solvers/base.h"
#include "conduits/upcxx.h"
#include "conduits/single.h"
#include "conduits/openmp.h"

Korali::Solver::Base::Base(Korali::Problem::Base* problem)
{
  _problem = problem;
  _verbose = false;
  _sampleCount = 1000;
	_maxGens = 200;

	N = _problem->_parameterCount;
}

void Korali::Solver::Base::run()
{
  _problem->initializeParameters();

  std::string conduitString = getenv("KORALI_CONDUIT");

  bool recognized = false;
  auto printSyntax = [](){fprintf(stderr, "[Korali] Use $export KORALI_CONDUIT={single,openmp,upcxx,mpi} to select a conduit.\n");};

  if (conduitString == "")
  {
			fprintf(stderr, "[Korali] Error: No sampling conduit was selected.\n");
			printSyntax();
			fprintf(stderr, "[Korali] Defaulting to 'single'.\n");
			conduitString = "single";
  }

  if (conduitString == "single") { _conduit = new Korali::Conduit::Single(this); recognized = true; }

  if (conduitString == "upcxx")
   {
    #ifdef _KORALI_USE_UPCXX
  	 _conduit = new Korali::Conduit::UPCXX(this);  recognized = true;
    #else
		 fprintf(stderr, "[Korali] Error: UPC++ conduit is not properly configured.\n");
		 printSyntax();
		 fprintf(stderr, "[Korali] Or reinstall Korali with the proper configuration to support UPC++.\n");
		 exit(-1);
    #endif
   }

  if (conduitString == "openmp") { _conduit = new Korali::Conduit::OpenMP(this); recognized = true; }

  if (recognized == false)
  {
			fprintf(stderr, "[Korali] Error: Unrecognized conduit '%s' selected.\n", conduitString.c_str());
			printSyntax();
			exit(-1);
  }

  _problem->_conduit = _conduit;
  _conduit->initialize();
}
