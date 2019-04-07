#include "korali.h"

using json = nlohmann::json;

Korali::Parameter::Base::Base()
{
 _name = "Unnamed Parameter";
 _lowerBound = 0.0;
 _upperBound = -1.0;
 _initialValue = 0.5;
 _initialStdDev = 0.001;
 _minStdDevChange = 0.0;
}

void Korali::Parameter::Base::initialize(int seed)
{
 _range = gsl_rng_alloc (gsl_rng_default);
 gsl_rng_set(_range, seed);

 checkBounds();
}

void Korali::Parameter::Base::checkBounds()
{
 if (_upperBound <= _lowerBound)
 {
  fprintf(stderr, "[Korali] Warning: Undefined or invalid lower/upper bounds for %s.\n", _name.c_str());
  fprintf(stderr, "[Korali] Tip: Use parameter.setBounds(lower,upper); to define bounds.\n");
  fprintf(stderr, "[Korali] Defaulting to [-100.0; 100.0]\n");
  _lowerBound = -100.0;
  _upperBound =  100.0;
 }

 if(_initialValue < _lowerBound || _initialValue > _upperBound)
  fprintf(stderr,"[Korali] Warning: Initial Value (%.4f) for \'%s\' is out of bounds (%.4f-%.4f).\n", _initialValue, _name.c_str(), _lowerBound, _upperBound);
}

json Korali::Parameter::Base::getConfiguration()
{
 auto js = json();
 return js;
}

void Korali::Parameter::Base::setConfiguration(json js)
{
  if (js.find("Name") != js.end()) if (js["Name"].is_string())
  { _name = js["Name"]; js.erase("Name"); }

  if (js.find("Bounds") != js.end())
  {
    json bounds = js["Bounds"];
    if (bounds.find("Lower") != bounds.end()) if (bounds["Lower"].is_number())
    { _lowerBound = bounds["Lower"]; bounds.erase("Lower"); }
  	if (bounds.find("Upper") != bounds.end()) if (bounds["Upper"].is_number())
    { _upperBound = bounds["Upper"]; bounds.erase("Upper"); }
  }

  if (js.find("Initial Mean") != js.end()) if (js["Initial Mean"].is_number())
  { _initialValue = js["Initial Mean"]; js.erase("Initial Mean"); }

  if (js.find("Initial StdDev") != js.end()) if (js["Initial StdDev"].is_number())
  { _initialStdDev = js["Initial StdDev"]; js.erase("Initial StdDev"); }

  if (js.find("Min StdDev Change") != js.end()) if (js["Min StdDev Change"].is_number())
  { _minStdDevChange = js["Min StdDev Change"]; js.erase("Min StdDev Change"); }
}
