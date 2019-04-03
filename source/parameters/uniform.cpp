#include "parameters/uniform.h"

using json = nlohmann::json;

Korali::Parameter::Uniform::Uniform(double min, double max) : Korali::Parameter::Uniform::Uniform("NoNameUniform", min, max) {}
Korali::Parameter::Uniform::Uniform(std::string name, double min, double max) : Korali::Parameter::Base::Base(name)
{
 _min = min;
 _max = max;
 setBounds(min, max);
}

double Korali::Parameter::Uniform::getDensity(double x)
{
 return gsl_ran_flat_pdf(x, _min, _max);
}

double Korali::Parameter::Uniform::getDensityLog(double x)
{
 if (x >= _min && x <= _max)
  return -log(_max-_min);
 return -INFINITY;
}

double Korali::Parameter::Uniform::getRandomNumber()
{
 return gsl_ran_flat(_range, _min, _max);
}

void Korali::Parameter::Uniform::printDetails()
{
 printf("Uniform(%.3g, %.3g)", _min, _max);
}

json Korali::Parameter::Uniform::serialize()
{
 auto j = this->Korali::Parameter::Base::serialize();
 j["Type"] = "Uniform";
 j["Minimum"] = _min;
 j["Maximum"] = _max;
 return j;
}
