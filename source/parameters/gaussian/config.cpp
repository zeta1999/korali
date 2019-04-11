#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Gaussian::Gaussian(nlohmann::json& js, int seed) : Korali::Parameter::Base::Base(js, seed)
{
 setConfiguration(js);
}

Korali::Parameter::Gaussian::Gaussian(double mean, double sigma, int seed) : Korali::Parameter::Base::Base(seed)
{
 _mean = mean;
 _sigma = sigma;
}

Korali::Parameter::Gaussian::~Gaussian()
{

}

json Korali::Parameter::Gaussian::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();

 js["Type"] = "Gaussian";
 js["Mean"] = _mean;
 js["Sigma"] = _sigma;

 return js;
}

void Korali::Parameter::Gaussian::setConfiguration(json& js)
{
 _mean  = consume(js, { "Mean" }, KORALI_NUMBER);
 _sigma = consume(js, { "Sigma" }, KORALI_NUMBER);
}
