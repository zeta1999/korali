#include "korali.h"

/************************************************************************/
/*                  Constructor / Destructor Methods                    */
/************************************************************************/

Korali::Problem::Direct::Direct(nlohmann::json& js) : Korali::Problem::Base::Base(js)
{
 setConfiguration(js);

 if (_k->_statisticalParameterCount != 0)
 {
  fprintf(stderr, "[Korali] Error: Direct Evaluation problem requires 0 statistical parameters.\n");
  exit(-1);
 }

 if (_model != KORALI_SINGLE)
 {
  fprintf(stderr, "[Korali] Error: Incorrect model for the Direct Evaluation problem.\n");
  exit(-1);
 }
}

Korali::Problem::Direct::~Direct()
{

}

/************************************************************************/
/*                    Configuration Methods                             */
/************************************************************************/

nlohmann::json Korali::Problem::Direct::getConfiguration()
{
 auto js = this->Korali::Problem::Base::getConfiguration();

 js["Objective"] = "Direct Evaluation";

 return js;
}

void Korali::Problem::Direct::setConfiguration(nlohmann::json& js)
{
}

/************************************************************************/
/*                    Functional Methods                                */
/************************************************************************/

double Korali::Problem::Direct::evaluateFitness(double* sample)
{
 if (isSampleOutsideBounds(sample)) return -DBL_MAX;
 return _k->_modelSingle(sample);
}
