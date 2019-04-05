#include "korali.h"
#include <chrono>

using json = nlohmann::json;

Korali::Engine* Korali::_k;

Korali::Engine::Engine(size_t seed)
{
 _seed = seed;
 if (_seed == 0)
 {
    std::time_t now_c = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() - std::chrono::nanoseconds(0));
  _seed  = std::chrono::nanoseconds(now_c).count();
 }
 gsl_rng_env_setup();

 N = 0;
 _referenceData = NULL;
 _referenceDataSize = 0;
 S = 1000;
 _maxGens = 200;
 _reportFrequency = 1;
 _verbosity = KORALI_NORMAL;
}

void Korali::Engine::run()
{
 _k = this;

 initializeParameters();

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

 if (conduitString == "single") { _conduit = new Korali::Conduit::Single(); recognized = true; }

 if (conduitString == "upcxx")
  {
   #ifdef _KORALI_USE_UPCXX
    _conduit = new Korali::Conduit::UPCXX();  recognized = true;
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
   _conduit = new Korali::Conduit::OpenMP(); recognized = true;
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

 _conduit->initialize();
}

void Korali::Engine::initializeParameters()
{
 // Initialize Parameter Priors
 for (size_t i = 0; i < N; i++) Parameters[i]->initializeDistribution(_seed+i);

 // Checking correct parameters for problem
 for (size_t i = 0; i < N; i++) Parameters[i]->checkBounds();
}

void Korali::Engine::addParameter(Korali::Parameter::Base* p)
{
 if(p->_name == "") p->setName("Parameter" + std::to_string(N));
 Parameters.push_back(p);
 N = Parameters.size();
}

double Korali::Engine::getPriorsLogProbabilityDensity(double *x)
{
  double logp = 0.0;
  for (size_t i = 0; i < N; i++) logp += Parameters[i]->getDensityLog(x[i]);
  return logp;
}

double Korali::Engine::getPriorsProbabilityDensity(double *x)
{
  double dp = 1.0;
  for (size_t i = 0; i < N; i++) dp *= Parameters[i]->getDensity(x[i]);
  return dp;
}

bool Korali::Engine::isSampleOutsideBounds(double* sample)
{
  for (size_t i = 0; i < N; i++) if (sample[i] < Parameters[i]->_lowerBound || sample[i] > Parameters[i]->_upperBound) return true;
  return false;
}

void Korali::Engine::setReferenceData(size_t nData, double* referenceData)
{
 _referenceDataSize = nData;
 _referenceData = referenceData;
}

json Korali::Engine::getConfiguration()
{
 auto js = json();
 js["parameterCount"] = N;
 js["referenceDataSize"] = _referenceDataSize;
 js["seed"] = _seed;
 js["verbosity"] = _verbosity;
 js["maxGens"] = _maxGens;
 js["sampleCount"] = S;
 js["reportFrequency"] = _reportFrequency;
 return js;
}

void Korali::Engine::setConfiguration(json js)
{
 N                  = js["parameterCount"];
 _referenceDataSize = js["referenceDataSize"];
 _seed              = js["seed"];
 _verbosity         = js["verbosity"];
 _maxGens           = js["maxGens"];
 S                  = js["sampleCount"];
 _reportFrequency   = js["reportFrequency"];
}
