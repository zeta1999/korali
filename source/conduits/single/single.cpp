#include "korali.h"

using json = nlohmann::json;

json Korali::Conduit::Single::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "Single";

 return js;
}

void Korali::Conduit::Single::setConfiguration(json js)
{
 this->Korali::Conduit::Base::setConfiguration(js);
}

Korali::Conduit::Single::Single(nlohmann::json& js) : Korali::Conduit::Base::Base(js)
{
 setConfiguration(js);
}

Korali::Conduit::Single::~Single()
{

}

void Korali::Conduit::Single::run()
{
 _k->_solver->run();
}

void Korali::Conduit::Single::evaluateSample(double* sampleArray, size_t sampleId)
{
 double fitness = _k->_problem->evaluateFitness(&sampleArray[_k->N*sampleId]);
 _k->_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::Single::checkProgress()
{

}

