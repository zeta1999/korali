#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Gamma::Gamma() : Korali::Parameter::Base::Base()
{
	_rate = 1.0;
	_shape = 1.0;
};

double Korali::Parameter::Gamma::getDensity(double x)
{
 return gsl_ran_gamma_pdf( x, _shape, _rate );
}

double Korali::Parameter::Gamma::getDensityLog(double x)
{
 if(x < 0) return -INFINITY;
 return - gsl_sf_lngamma(_shape) - _shape*log(_rate) + (_shape-1)*log(x) - x/_rate;
}

double Korali::Parameter::Gamma::getRandomNumber()
{
 return gsl_ran_gamma(_range, _shape, _rate);
}

json Korali::Parameter::Gamma::getConfiguration()
{
 auto js = this->Korali::Parameter::Base::getConfiguration();
 return js;
}

void Korali::Parameter::Gamma::setConfiguration(json js)
{
  if (js.find("Distribution") != js.end())
  {
    json dist = js["Distribution"];
    if (dist.find("Shape") != dist.end()) if (dist["Shape"].is_number())
    { _shape = dist["Shape"]; dist.erase("Shape"); }
    if (dist.find("Rate") != dist.end()) if (dist["Rate"].is_number())
    { _rate = dist["Rate"]; dist.erase("Rate"); }
  }

	this->Korali::Parameter::Base::setConfiguration(js);
}
