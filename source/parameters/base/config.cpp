#include "korali.h"

using json = nlohmann::json;

json Korali::Parameter::Base::getConfiguration()
{
 auto js = json();
 return js;
}

void Korali::Parameter::Base::setConfiguration(json js)
{
 _name = consume(js, { "Name" }, KORALI_STRING, "Unnamed Parameter");

 auto typeString = consume(js, { "Type" }, KORALI_STRING, "Computational");
 if (typeString == "Computational")  _type = KORALI_COMPUTATIONAL;
 if (typeString == "Statistical")    _type = KORALI_STATISTICAL;

 _lowerBound = consume(js, { "Minimum" }, KORALI_NUMBER);
 _upperBound = consume(js, { "Maximum" }, KORALI_NUMBER);

 if (_upperBound <= _lowerBound)
 {
  fprintf(stderr, "[Korali] Error: Invalid lower/upper bounds for %s.\n", _name.c_str());
  exit(-1);
 }

 _initialValue = (_lowerBound + _upperBound) * 0.5;
 _initialStdDev = (_upperBound - _lowerBound) * 0.25;

 _initialValue  = consume(js, { "Initial Mean"   }, KORALI_NUMBER, std::to_string(_initialValue));
 _initialStdDev = consume(js, { "Initial StdDev" }, KORALI_NUMBER, std::to_string(_initialStdDev));

 if(_initialValue < _lowerBound || _initialValue > _upperBound)
 {
  fprintf(stderr,"[Korali] Error: Initial Value (%.4f) for \'%s\' is out of bounds (%.4f-%.4f).\n", _initialValue, _name.c_str(), _lowerBound, _upperBound);
  exit(-1);
 }

 _minStdDevChange = consume(js, { "Initial StdDev" }, KORALI_NUMBER, "0.0");
}
