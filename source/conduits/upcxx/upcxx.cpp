#ifdef _KORALI_USE_UPCXX

#include "korali.h"

Korali::Conduit::UPCXX* _ux;

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Conduit::UPCXX::UPCXX(nlohmann::json& js) : Korali::Conduit::Base::Base(js)
{
 setConfiguration(js);

 _ux = this;
 _rankId = upcxx::rank_me();
 _rankCount = upcxx::rank_n();

 if (_rankCount < 2)
 {
  fprintf(stderr, "[Korali] Error: Running Korali's UPCxx Conduit with less than 2 ranks is not allowed.\n");
  exit(-1);
 }

 samplePtr = upcxx::new_array<double>(_k->N);
 upcxx::dist_object<upcxx::global_ptr<double>> p(samplePtr);
 upcxx::barrier();

 if (_rankId == 0)
 {
  samplePtrGlobal = (upcxx::global_ptr<double>*) calloc (_rankCount, sizeof(upcxx::global_ptr<double>));
  for (int i = 0; i < _rankCount; i++)samplePtrGlobal[i] = p.fetch(i).wait();
 }

 upcxx::barrier();
}

Korali::Conduit::UPCXX::~UPCXX()
{
 upcxx::finalize();
}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

json Korali::Conduit::UPCXX::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "UPC++"

 return js;
}

void Korali::Conduit::UPCXX::setConfiguration(nlohmann::json js)
{
  _rankId = 0;
  _rankCount = 1;
  _continueEvaluations = true;
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

void Korali::Conduit::UPCXX::run()
{
 // Supervisor
 if (_rankId == 0)
 {
  for (int i = 1; i < _rankCount; i++) _workers.push(i);
 _k->_solver->run();
  for (int i = 1; i < _rankCount; i++) upcxx::rpc_ff(i, [](){_ux->_continueEvaluations = false;});
 }
 // Workers
 else while(_continueEvaluations) upcxx::progress();

 upcxx::barrier();
}

void Korali::Conduit::UPCXX::evaluateSample(double* sampleArray, size_t sampleId)
{
 while(_workers.empty()) upcxx::progress();
 int workerId = _workers.front(); _workers.pop();
 auto put = upcxx::rput(&sampleArray[sampleId*_k->N], samplePtrGlobal[workerId], _k->N);

 put.then([workerId, sampleId]()
 {
 auto doWorker = upcxx::rpc(workerId, [](size_t sampleId)
 {
 double fitness = _k->_problem->evaluateFitness(_ux->samplePtr.local());
 upcxx::rpc_ff(0, [](size_t sampleId, double fitness)
 { _k->_solver->processSample(sampleId, fitness); },sampleId, fitness);
 }, sampleId);

 doWorker.then([workerId](){_ux->_workers.push(workerId);});
 });
}

void Korali::Conduit::UPCXX::checkProgress()
{
 upcxx::progress();
}

bool Korali::Conduit::UPCXX::isRoot()
{
 return _rankId == 0;
}

#endif // #ifdef _KORALI_USE_UPCXX
