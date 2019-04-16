#include "korali.h"

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Conduit::Sequential::Sequential(nlohmann::json& js) : Korali::Conduit::Base::Base(js)
{
 setConfiguration(js);
}

Korali::Conduit::Sequential::~Sequential()
{

}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

nlohmann::json Korali::Conduit::Sequential::getConfiguration()
{
 auto js = this->Korali::Conduit::Base::getConfiguration();

 js["Type"] = "Sequential";

 return js;
}

void Korali::Conduit::Sequential::setConfiguration(nlohmann::json js)
{
 this->Korali::Conduit::Base::setConfiguration(js);
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

void Korali::Conduit::Sequential::run()
{
 _k->_solver->run();
}

void Korali::Conduit::Sequential::evaluateSample(double* sampleArray, size_t sampleId)
{
 double fitness = _k->_problem->evaluateFitness(&sampleArray[_k->N*sampleId]);
 _k->_solver->processSample(sampleId, fitness);
}

void Korali::Conduit::Sequential::checkProgress()
{

}

bool Korali::Conduit::Sequential::isRoot()
{
 return true;
}
