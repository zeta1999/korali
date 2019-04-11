#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Exponential::Exponential(nlohmann::json& js, int seed) : Korali::Parameter::Base::Base(js, seed)
{
 setConfiguration(js);
}

Korali::Parameter::Exponential::~Exponential()
{

}

json Korali::Parameter::Exponential::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Exponential";
 js["Mean"] = _mean;

 return js;
}

void Korali::Parameter::Exponential::setConfiguration(json& js)
{
 _mean = consume(js, { "Mean" }, KORALI_NUMBER);
}

double Korali::Parameter::Exponential::getDensity(double x)
{
 return  gsl_ran_exponential_pdf(x, _mean);
}

double Korali::Parameter::Exponential::getDensityLog(double x)
{
 if (x < 0) return -INFINITY;
 return - log(_mean) - x/_mean;
}

double Korali::Parameter::Exponential::getRandomNumber()
{
 return gsl_ran_exponential(_range, _mean);
}

