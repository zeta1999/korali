#ifndef _KORALI_H_
#define _KORALI_H_

#include "prior.h"
#include <vector>

namespace Korali
{

class Engine
{
 public:
  Engine(){};
  void AddPrior(int arg1);

 private:
  std::vector<Prior*> priors;
};


} // namespace Korali

#endif // _KORALI_H_
