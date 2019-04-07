#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Uniform::Uniform() : Korali::Parameter::Base::Base()
{
	_min = -1.0;
	_max = +1.0;
};

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

json Korali::Parameter::Uniform::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Uniform::setConfiguration(json js)
{
  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Minimum") != dist.end()) if (dist["Minimum"].is_number())
    { _min = dist["Minimum"]; dist.erase("Minimum"); }
    if (dist.find("Maximum") != dist.end()) if (dist["Maximum"].is_number())
    { _max = dist["Maximum"]; dist.erase("Maximum"); }
  }

  _lowerBound = _min;
  _upperBound = _max;

	this->Korali::Parameter::Base::setConfiguration(js);
}

