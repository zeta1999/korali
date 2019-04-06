#include "parameters/gamma.h"

using json = nlohmann::json;

Korali::Parameter::Gamma::Gamma() : Korali::Parameter::Base::Base() {};

//Korali::Parameter::Gamma::Gamma(double shape, double rate) : Korali::Parameter::Gamma::Gamma("NoNameGamma", shape, rate) {}
//Korali::Parameter::Gamma::Gamma(std::string name, double shape, double rate) : Korali::Parameter::Base::Base(name)
//{
// _shape = shape;
// _rate = rate;
//}

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
	this->Korali::Parameter::Base::setConfiguration(js);
}
